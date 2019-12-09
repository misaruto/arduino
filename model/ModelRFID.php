<?php
class RFID {
	private $token;
	private $codAluno;

	function __construct()
	{
		$token = "";
		$codAluno = "";
	}
	
    /**
     * @return mixed
     */
    public function getToken()
    {
        return $this->token;
    }

    /**
     * @param mixed $token
     *
     * @return self
     */
    public function setToken($token)
    {
        $this->token = $token;

        return $this;
    }

    /**
     * @return mixed
     */
    public function getCodAluno()
    {
        return $this->codAluno;
    }

    /**
     * @param mixed $codAluno
     *
     * @return self
     */
    public function setCodAluno($codAluno)
    {
        $this->codAluno = $codAluno;

        return $this;
    }
}