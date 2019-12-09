<?php
require "../model/ModelAcesso.php";
require "../model/ModelAluno.php";

$data = array();

if($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['acao'])){
    switch ($_POST['acao']) {
        case "cadastrarAluno":
            $aluno = new Aluno();
            $aluno->setNome($_POST['nome']);
            $aluno->setCpf($_POST['cpf']);
            $aluno->setEmail($_POST['email']);
            cadastrarAluno($aluno);
            break;

        default:
            $data['success'] = false;
            $data['message'] = 'Ação invalida!';

            header("Content-Type: application/json; charset=UTF-8");
            echo json_encode($data);
            break;
    }
}elseif($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['acao'])){
    switch ($_GET['acao']) {
        case "listarAlunosBolsistas":
            listarAlunosBolsistas();
            break;

        case "controleCatraca":
            $aluno = new Aluno();
            $aluno->setCpf($_GET['cpf']);
            controleCatraca($aluno);
            break;

        default:
            $data['success'] = false;
            $data['message'] = 'Ação invalida!';

            header("Content-Type: application/json; charset=UTF-8");
            echo json_encode($data);
            break;
    }
}else{
    $data['success'] = false;
    $data['message'] = 'Error!';

    header("Content-Type: application/json; charset=UTF-8");
    echo json_encode($data);
}

function cadastrarAluno($aluno){
    try{
        $acesso = new Acesso();
        $prepare = $acesso->conexao()->prepare("INSERT INTO Aluno(nome, cpf, email) VALUES(:nome, :cpf, :email)");
        $prepare->bindValue(':nome', $aluno->getNome());
        $prepare->bindValue(':cpf', $aluno->getCpf());
        $prepare->bindValue(':email', $aluno->getEmail());
        $prepare->execute();

        $data['success'] = true;
        $data['message'] = 'Success!';
    }catch(Exception $e){
        $data['success'] = false;
        $data['message'] = 'Erro: ' .$e->getMessage();
    }

    header("Content-Type: application/json; charset=UTF-8");
    echo json_encode($data);
}

function listarAlunosBolsistas(){
    try{
        $alunos = array();
        $acesso = new Acesso();
        $prepare = $acesso->conexao()->prepare("SELECT *, COUNT(*) AS TOTAL FROM VisaoGeralBolsistas");
        $prepare->execute();
        while($resultado = $prepare->fetch(PDO::FETCH_ASSOC)){
            $aluno = array("cod" => $resultado['codAluno'],
                            "nome" => $resultado['nome'], 
                            "cpf" => $resultado['cpf'],
                            "email" => $resultado['email'],
                            "bolsaAtiva" => $resultado['bolsaAtiva'],
                            "credito" => $resultado['credito']);
            array_push($alunos, $aluno);
        }

        $data['success'] = true;
        $data['data'] = $alunos;

        
    }catch(Exception $e){
        $data['success'] = false;
        $data['message'] = 'Erro: ' .$e->getMessage();
    }

    header("Content-Type: application/json; charset=UTF-8");
    echo json_encode($data);
}


function controleCatraca($aluno){
    try{
        $acesso = new Acesso();
        $prepare = $acesso->conexao()->prepare("SELECT VisaoGeralBolsistas.credito AS credito, COUNT(*) AS total FROM VisaoGeralBolsistas WHERE VisaoGeralBolsistas.cpf = :cpf");
        $prepare->bindValue(':cpf', $aluno->getCpf());
        $prepare->execute();
        $resultado = $prepare->fetch(PDO::FETCH_ASSOC);

        if($resultado['total'] > 0){
            if($resultado['credito'] > 0){
                $prepare = $acesso->conexao()->prepare("UPDATE Bolsa 
                                                        INNER JOIN Aluno ON Aluno.codAluno = Bolsa.codAluno 
                                                        SET Bolsa.credito = Bolsa.credito - 1 
                                                        WHERE Aluno.cpf = :cpf");
                $prepare->bindValue(':cpf', $aluno->getCpf());
                $prepare->execute();
                $resultado = $prepare->fetch(PDO::FETCH_ASSOC);

                $data['success'] = true;
                $data['message'] = "Permitido!";
                $data['code'] = 200;
            }else{
                $data['success'] = false;
                $data['message'] = "Saldo insulficiente!";
                $data['code'] = 401;
            }
        }else{
            $data['success'] = false;
            $data['message'] = "CPF invalido!";
            $data['code'] = 400;
        }
            
    }catch(Exception $e){
        $data['success'] = false;
        $data['message'] = 'Erro: ' .$e->getMessage();
    }

    header("Content-Type: application/json; charset=UTF-8");
    echo json_encode($data);
}