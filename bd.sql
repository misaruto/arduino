CREATE TABLE Aluno (
    codAluno INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    nome VARCHAR(100) NOT NULL,
    cpf VARCHAR(20) NOT NULL,
    email VARCHAR(100) NOT NULL
);

CREATE TABLE Bolsa (
    idBolsa INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    ativo TINYINT(1) COMMENT "0. FALSE / 1. TRUE",
    credito INT NOT NULL DEFAULT 0,
    codAluno INT NOT NULL,
    FOREIGN KEY (codAluno) REFERENCES Aluno(codAluno)
);

CREATE TABLE Logs (
    codLog INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    codAluno INT NOT NULL,
    horario DATETIME NOT NULL,
    FOREIGN KEY (codAluno) REFERENCES Aluno(codAluno)
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

CREATE TABLE RFID (
    tokenRFID VARCHAR(20) NOT NULL PRIMARY KEY,
    codAluno INT NOT NULL,
    FOREIGN KEY (codAluno) REFERENCES Aluno(codAluno) ON DELETE CASCADE
);

