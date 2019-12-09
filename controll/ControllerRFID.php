<?php
require "../model/ModelAcesso.php";
require "../model/ModelRFID.php";

$data = array();

if($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['acao'])){
    switch ($_GET['acao']) {
        case "verificarPermissao":
            $rfid = new RFID();
            $rfid->setToken($_GET['token']);
            verificarPermissao($rfid);
            break;

        case "verificarTokens":
            verificarTokens();
            break;

        default:
            $data['success'] = false;
            $data['message'] = 'Acao invalida!';

            header("Content-Type: application/json; charset=UTF-8");
            echo json_encode($data);
            break;
    }
}else{
    $data['success'] = false;
    $data['message'] = 'Ops! Parametros incompletos';

    header("Content-Type: application/json; charset=UTF-8");
    echo json_encode($data);
}

function verificarPermissao($rfid){
    try{
        $acesso = new Acesso();
        $prepare = $acesso->conexao()->prepare("SELECT bolsaAtiva FROM VisaoGeral WHERE tokenRFID = :token");
        $prepare->bindValue(':token', $rfid->getToken());
        $prepare->execute();
        $resultado = $prepare->fetch(PDO::FETCH_ASSOC);

        if($resultado['bolsaAtiva'] == true){
            $data['success'] = true;
            $data['message'] = 'Acesso permitido!';
        }else{
            $data['success'] = false;
            $data['message'] = 'Acesso negado!';
        }
    }catch(Exception $e){
        $data['success'] = false;
        $data['message'] = 'Erro: ' .$e->getMessage();
    }

    header("Content-Type: application/json; charset=UTF-8");
    echo json_encode($data);
}

function verificarTokens(){
    try{
        $tokens = array();
        $acesso = new Acesso();
        $prepare = $acesso->conexao()->prepare("SELECT tokenRFID FROM rfid");
        $prepare->execute();
        while($resultado = $prepare->fetch(PDO::FETCH_ASSOC)){
            array_push($tokens, $resultado['tokenRFID']);
        }

        $data['success'] = true;
        $data['message'] = $tokens;
    }catch(Exception $e){
        $data['success'] = false;
        $data['message'] = 'Erro: ' .$e->getMessage();
    }

    header("Content-Type: application/json; charset=UTF-8");
    echo json_encode($data);
}