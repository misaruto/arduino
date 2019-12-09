<?php
/**
 * 
 */
class Aluno
{
	private $cod;
	private $nome;
	private $cpf;
	private $email;

	function __construct()
	{
		$nome = "";
		$cpf = "";
		$email = "";
	}




    /**
     * @return mixed
     */
    public function getCod()
    {
        return $this->cod;
    }

    /**
     * @param mixed $cod
     *
     * @return self
     */
    public function setCod($cod)
    {
        $this->cod = $cod;

        return $this;
    }

    /**
     * @return mixed
     */
    public function getNome()
    {
        return $this->nome;
    }

    /**
     * @param mixed $nome
     *
     * @return self
     */
    public function setNome($nome)
    {
        $this->nome = $nome;

        return $this;
    }

    /**
     * @return mixed
     */
    public function getCpf()
    {
        return $this->cpf;
    }

    /**
     * @param mixed $cpf
     *
     * @return self
     */
    public function setCpf($cpf)
    {
        $this->cpf = $cpf;

        return $this;
    }

    /**
     * @return mixed
     */
    public function getEmail()
    {
        return $this->email;
    }

    /**
     * @param mixed $email
     *
     * @return self
     */
    public function setEmail($email)
    {
        $this->email = $email;

        return $this;
    }
}