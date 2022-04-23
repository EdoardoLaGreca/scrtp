# powershell version of build.sh

$cc="gcc"
$cflags="-std=c90 -Wall -Wpedantic -O3"
$cflags_dbg="-std=c90 -Wall -Wpedantic -Og -g"
$check_libs="../script/check_libs.sh"

# libraries to link with, without prefixes or suffixes (like "-l", "lib" or ".so")
$libs="glfw GL"


function gen_ld_args {
}

function check_deps {
}

function compile {
}

function compile_dbg {
}

function link {
}

function clean {
}

function build {

}

function build_dbg {
}


# check if args[0] corrisponds to a function, if yes execute it, otherwise print "usage" help
if ((Get-ChildItem function: -Name) -contains $args[0]) {
    &$args
}
else {
    Write-Output "Usage: $($MyInvocation.MyCommand.Name) [checkdeps|compile|compile_dbg|link|clean|build|build_dbg|all]"
}