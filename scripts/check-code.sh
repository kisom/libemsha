#!/usr/bin/env sh

######################################################################
# @author      : kyle (kyle@midgard)
# @file        : check-code
# @created     : Tuesday Oct 17, 2023 22:39:39 PDT
#
# @description : 
######################################################################


run_clang_tidy () {
	sources="${1:-*.cc}"
	echo "[+] clang-tidy ${sources}"

	if [ ! -e compile_commands.json ]
	then
		echo "[!] compile_commands.json not found" > /dev/stderr
		candidate=$(find -name compile_commands.json | head)

		if [ -z "${candidates}" ]
		then
			echo "[!] no suitable candidates found; can't proceed" > /dev/stderr
			exit 1
		fi

		echo "[+] compile_commands.json candidate: $candidate"
		ln -s ${candidate} .
		echo "[+] if this isn't correct, you will need to manually link it"
	fi

	clang-tidy ${sources}
}


run_cppcheck () {
	sources="${1:-*.cc}"
	echo "[+] cppcheck ${sources}"

	cppcheck  --enable=all --suppress=unusedFunction --suppress=missingIncludeSystem -I. ${sources}
}


run_trunk () {
	sources="${1:-}"
	echo "[+] trunk check ${sources}"

	trunk check --filter clang-tidy ${sources}
}


main () {
	command="${1:-usage}"
	shift

	case ${command} in
		clang-tidy)	run_clang_tidy $@ ;;
		cppcheck)	run_cppcheck $@ ;;
		trunk)		run_trunk $@ ;;
		*)
			echo "[!] scanner ${command} isn't supported" > /dev/stderr
			exit 1
			;;
	esac
}

main $@

