<?php

class Acesso
{

	function __construct(){
		if (!defined('DSN')) define('DSN','mysql:host=localhost;dbname=arduino');
		if (!defined('USERNAME')) define('USERNAME','root');
		if (!defined('SENHA')) define('SENHA','');
	}

	function conexao(){
		$conexao = null;
		try {
			$conexao = new PDO(DSN, USERNAME, SENHA);
		}catch (Exception $e){
			echo 'ERRO: ' .$e->getCode() .' Mensagem: ' .$e->getMessage();
		}
		return $conexao;
	}

}