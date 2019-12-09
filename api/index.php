<?php

$urls = "../controll/ControllerAluno.php";
ob_start();
include $urls;
$teste = ob_get_contents();
ob_clean();

$js = json_decode($teste);
echo $js->code;