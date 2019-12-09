<!DOCTYPE html>
<html lang="en">

<head>

  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <meta name="description" content="">
  <meta name="author" content="">

  <title>Cadastrar Aluno</title>

  <!-- Custom fonts for this template-->
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">
  <link href="../assets/vendor/fontawesome-free/css/all.min.css" rel="stylesheet" type="text/css">
  <link href="https://fonts.googleapis.com/css?family=Nunito:200,200i,300,300i,400,400i,600,600i,700,700i,800,800i,900,900i" rel="stylesheet">

  <!-- Custom styles for this template-->
  <link href="../assets/css/sb-admin-2.css" rel="stylesheet">

</head>

<body class="bg-gradient-primary">

  <div class="container">

    <div class="card o-hidden border-0 shadow-lg my-5">
      <div class="card-body p-0">
        <!-- Nested Row within Card Body -->
        <div class="row">
          <div class="col-lg-5 d-none d-lg-block bg-register-image p-5"></div>
          <div class="col-lg-7">
            <div class="p-5">
              <div class="text-center">
                <h1 class="h4 text-gray-900 mb-4">Cadastrar Aluno!</h1>
              </div>
              <form class="user" id="target">
                <div id="checks"></div>
                <div class="form-group row mt-3">
                  <div class="col-sm-6 mb-3 mb-sm-0">
                    <input type="text" class="form-control form-control-user" id="nomeAluno" placeholder="Nome">
                  </div>
                  <div class="col-sm-6">
                    <input type="text" class="form-control form-control-user" id="cpfAluno" placeholder="CPF">
                  </div>
                </div>
                <div class="form-group">
                  <input type="email" class="form-control form-control-user" id="emailAluno" placeholder="Email">
                </div>
             
                <input type="submit" class="btn btn-primary btn-user btn-block" value="Cadastrar">
                <hr>
              </form>
            </div>
          </div>
        </div>
      </div>
    </div>

  </div>

  <!-- Bootstrap core JavaScript-->
  <script src="../assets/vendor/jquery/jquery.min.js"></script>
  <script src="../assets/vendor/bootstrap/js/bootstrap.bundle.min.js"></script>

  <!-- Core plugin JavaScript-->
  <script src="../assets/vendor/jquery-easing/jquery.easing.min.js"></script>

  <!-- Custom scripts for all pages-->
  <script src="../assets/js/sb-admin-2.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/sweetalert2@9"></script>


  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js"></script>
  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js"></script>
	<script type="text/javascript" charset="utf-8">
		$("#target").submit(function( event ) {
			$.ajax({
				url: "../controll/ControllerAluno.php",
				method: "POST",
				data: {
					"nome": $("#nomeAluno").val(),
					"email": $("#emailAluno").val(),
					"cpf": $("#cpfAluno").val(),
					"acao": "cadastrarAluno"},
				success: function(result) {
					Swal.fire({
            icon: 'success',
            title: 'Cadastrado com sucesso!',
            showConfirmButton: false,
            timer: 1500
          })
          $('#target')[0].reset();
				},
				error: function(result) {
					Swal.fire({
            icon: 'error',
            title: 'Ops!',
            showConfirmButton: false,
            timer: 1500
          })
        }
			})
			event.preventDefault();
		});
	</script>

</body>

</html>
