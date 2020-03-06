# Tentativa de modelo</h1>

## Como deve funcionar:</h2>

    1 - Instancia uma conexão com o front-end(a catraca), e então começa a ouvir requisições

    2 - Caso haja uma requisição, recupere os dados vindos da catraca, faça a validação

    dele (para impedir ataques e verificar a consistência dos dados)

    3 - Fazer a querry com o database e verificar se os dados batem

    4 - Devolver uma resposta para o front-end

## Formato da requisição (protocolo)

### opcode;cpf

## Sendo os opcodes

### Opcodes de requisição

    0 - Permitir entrada

    1 - Informar entrada

### Possíveis respostas

    0 - Erro inerno <br>

    2 - Ok!

<p><strong>Token de autenticação usado para autenticar a catraca</strong></p>

<h2> Possíveis respostas:</h2>
    <p>1: Usuário existente e autorizado</p>
    <p>2: Usuário existente porém não autorizado</p>
    <p>3: Usuário inexistente</p>