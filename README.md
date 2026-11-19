# unimatrix

## Dependências

O `run-kernel.sh` compila o kernel em **32 bits** (C e Assembly), liga os objetos em
um binário *flat* e abre o resultado no emulador. Para isso são necessárias as
seguintes ferramentas:

| Ferramenta | Usada em | Para que serve |
|---|---|---|
| **bash** e **coreutils** (`cat`) | passo 5 | concatenar boot sector + kernel |
| **gcc** com suporte a `-m32` | passo 1 | compilar `kernel.c` para i386 |
| **nasm** | passos 2 e 4 | montar `kernel_entrada.asm` e `bootsect.asm` |
| **binutils** (`ld`) com suporte a `elf_i386` | passo 3 | linkar o kernel |
| **qemu-system-i386** | passo 6 | emular a máquina e testar a imagem |

> O GCC precisa aceitar `-m32` (multilib) mesmo com `-ffreestanding`. O `ld`
> precisa do *emulation* `elf_i386` (vem habilitado na maioria das distribuições).

### Linux

Comandos de instalação por distribuição:

**Arch Linux**

```sh
sudo pacman -S gcc nasm binutils qemu-system-x86
```

**Debian / Ubuntu**

```sh
sudo apt update
sudo apt install build-essential gcc-multilib nasm binutils qemu-system-x86
```

**Fedora**

```sh
sudo dnf install gcc nasm binutils qemu-system-x86
```

Se o `gcc -m32` reclamar de bibliotecas de 32 bits, instale também
`glibc-devel.i686` e `libgcc.i686` (Fedora) ou `gcc-multilib` (Debian/Ubuntu).

Para conferir se está tudo disponível:

```sh
gcc -m32 -ffreestanding -x c -c /dev/null -o /tmp/teste32.o
nasm -v
ld -V | grep elf_i386
qemu-system-i386 --version
```

### Windows

O script é **bash** e o toolchain gera binários **ELF i386**, então o caminho
recomendado no Windows é usar o **WSL2** (Linux real rodando dentro do Windows).
Compilar nativamente com MSYS2/MinGW não funciona bem, pois o `ld` geraria
objetos PE/COFF em vez de ELF.

**1. Instalar o WSL2** (PowerShell como administrador):

```powershell
wsl --install -d Ubuntu
```

Reinicie se for pedido e abra o Ubuntu.

**2. Instalar as dependências dentro do WSL** (mesmos comandos do Linux):

```sh
sudo apt update
sudo apt install build-essential gcc-multilib nasm binutils qemu-system-x86 dos2unix
```

**3. Rodar o kernel:**

```sh
dos2unix run-kernel.sh      # evita erro de CRLF no shebang
chmod +x run-kernel.sh
./run-kernel.sh
```

No **Windows 11** a janela do QEMU aparece normalmente (WSLg). No **Windows 10**,
instale o [QEMU para Windows](https://www.qemu.org/download/#windows) e execute a
imagem gerada a partir do PowerShell:

```powershell
qemu-system-i386.exe -fda imagem-so.bin
```

Alternativa: usar Docker (Linux) para compilar e rodar o QEMU no host Windows, ou
apenas para gerar `imagem-so.bin` e abri-la com o QEMU do Windows.

> **Atenção (Windows)**: se o repositório for clonado com `core.autocrlf=true`, o
> Git converte o `run-kernel.sh` para CRLF e o script deixa de rodar. Nesse caso
> use `dos2unix run-kernel.sh` (ou rode com `bash run-kernel.sh`).

## Rodar kernel

Para rodar o kernel sem precisar repetir os comandos, use o arquivo: `run-kernel.sh`.

Talvez seja necessário dar permissão de execução a ele:

```sh
chmod +x run-kernel.sh
```

Então só rodar:

```sh
./run-kernel.sh
```

> **Nota**: foi testado no Arch Linux.
