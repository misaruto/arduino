CREATE TABLE Quartos ( 
   codQuarto INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
   numero INT(4) NOT NULL , 
   totalVagas INT(2) NOT NULL , 
   vagasOcupadas INT(2) NOT NULL DEFAULT '0'
);


CREATE TABLE Catracas ( 
  codCatraca INT NOT NULL AUTO_INCREMENT PRIMARY KEY, 
  localizacao TEXT NOT NULL , 
  validador VARCHAR(32) NOT NULL
);


CREATE TABLE Aluno (
    codAluno INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    nome VARCHAR(100) NOT NULL,
    cpf VARCHA20) NOT NULL,
    email VARCHAR(100) NOT NULL
);


CREATE TABLE Alunos_Quartos (
codAlunoQuarto INT NOT NULL AUTO_INCREMENT , 
codAluno INT NOT NULL ,
codQuarto INT NOT NULL , 
PRIMARY KEY (codAlunoQuarto), 
INDEX Aluno (codAluno),
INDEX Quarto (codQuarto)
);



CREATE TABLE Bolsa (
    idBolsa INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    ativo TINYINT(1) COMMENT "0. FALSE / 1. TRUE",
    credito INT NOT NULL DEFAULT 0,
    codAluno INT NOTR( NULL,
    FOREIGN KEY (codAluno) REFERENCES Aluno(codAluno)
);

CREATE TABLE Logs (
    codLog INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    codAluno INT NOT NULL,
    horario DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    codCatraca INT NOT NULL,
    FOREIGN KEY (codAluno) REFERENCES Aluno(codAluno),
    FOREIGN KEY (codCatraca) REFERENCES Catracas(codCatraca)
);


CREATE TABLE RFID (
    tokenRFID VARCHAR(20) NOT NULL PRIMARY KEY,
    codAluno INT NOT NULL,
    FOREIGN KEY (codAluno) REFERENCES Aluno(codAluno) ON DELETE CASCADE
);


CREATE VIEW VisaoGeralBolsistas AS
SELECT      Aluno.codAluno AS codAluno,
            Aluno.nome AS nome,
            Aluno.cpf AS cpf,
            Aluno.email AS email,
            Bolsa.ativo AS bolsaAtiva,
            Bolsa.credito AS credito
FROM        Aluno 
INNER JOIN  Bolsa  ON   Aluno.codAluno = Bolsa.codAluno
GROUP BY    Aluno.codAluno;


DELIMITER //
CREATE TRIGGER logPassaPelaCatraca AFTER UPDATE ON Bolsa
  FOR EACH ROW
  BEGIN
    INSERT INTO Logs(codAluno, horario) VALUES(OLD.codAluno, NOW());
  END;
//

DELIMITER //
CREATE TRIGGER gerarBolsa AFTER INSERT ON Aluno
  FOR EACH ROW
  BEGIN
    INSERT INTO Bolsa(ativo, codAluno) VALUES(1, NEW.codAluno);
  END;
//

DELIMETER //
CREATE TRIGGER incrementaQuarto AFTER INSERT ON Aluno
  FOR EACH ROW 
    BEGIN 
      UPDATE Quartos INNER JOIN Aluno ON NEW.codQuarto = Quartos.codQuarto 
      SET Quartos.vagasOcupadas = (Quartos.vagasOcupadas +1);   
END;
//
DELIMETER ;


DELIMETER //
CREATE TRIGGER decrementaQuarto AFTER DELETE ON Aluno 
  FOR EACH ROW 
  BEGIN 
    UPDATE Quartos INNER JOIN Aluno ON OLD.codQuarto = Quartos.codQuarto 
    SET Quartos.vagasOcupadas = (Quartos.vagasOcupadas -1); 
END;
//
DELIMETER ;

CREATE VIEW VerificaAluno AS 
  SELECT tokenRFID as token,
    CASE 
      WHEN Bolsa.ativo = 1 THEN "2"
      WHEN Bolsa.ativo = 0 THEN "1" 
      ELSE "0" END 
      AS statuscode 
      FROM Aluno,Bolsa,RFID
    WHERE RFID.codAluno = Aluno.codAluno 
    AND Bolsa.codAluno = Aluno.codAluno;