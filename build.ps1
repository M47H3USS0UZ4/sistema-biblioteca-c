param(
    [switch]$Run
)

# Falha imediatamente em qualquer erro não tratado.
$ErrorActionPreference = 'Stop'

# Define nome do executável e lista de fontes C.
$OutputName = 'library_system.exe'
$Sources = @(
    'library_system.c',
    'library_catalog.c',
    'io_utils.c'
)

# Compila o projeto.
Write-Host 'Compilando projeto...'
gcc @Sources -o $OutputName

# Interrompe em caso de falha na compilação.
if ($LASTEXITCODE -ne 0) {
    throw 'Falha na compilação.'
}

Write-Host "Build concluído: $OutputName"

# Executa o binário caso o parâmetro -Run tenha sido informado.
if ($Run) {
    Write-Host 'Executando aplicação...'
    & (Join-Path $PSScriptRoot $OutputName)
}
