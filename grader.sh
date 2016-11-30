#!/bin/bash
############################
# CSCI3150 File System Grader
############################
EXCUTABLE=vfs
INPUT_DIR=./testcases
OUTPUT_DIR=./testcases
VFS_DIR=./.vfsdata
TEST="case"


# have fun with colors and styles :)
# ref: http://misc.flogisoft.com/bash/tip_colors_and_formattin
BD="\033[1m"
BDED="\033[21m"
RED="\033[31m"
GREEN="\033[92m"
YELLOE="\E[33m"
ED="\e[0m"
DIFF_FLAG="-Z -b -B"

############################
# Main
function main {
	# check EXCUTABLE 
	if [ ! -f ${EXCUTABLE} ]
	then
		echo -e "${BD}${RED}Cannot find excutable ${EXCUTABLE} !${ED}"
        	exit -1
	fi

	# Create vfs folder.
	if [ ! -d ${VFS_DIR} ]; then
	mkdir -p ${VFS_DIR} || { echo -e "${BD}${RED}Cannot create vfs directory \"${VFS_DIR}\"${ED}"; exit -1; }
	fi
	
	arg1=$1
	if [ "$arg1" == "PartA" ]
	then
		echo -e "${BD}${YELLOE}Now test correctness${ED}"
		TESTCASE=$2
		correctness  $TESTCASE
	elif [ "$arg1" == "PartB" ]
	then
		echo -e "${BD}${YELLOE}Now test performance${ED}"
		TESTCASE=$2
		performance  $TESTCASE
	else
		echo -e "Usage: ./grader.sh [PartA or PartB] [testcase]"
	fi
}

function correctness {
	if [ -n "$1" ]
	then
		TESTCASE=$1
		if [ ! -d ${INPUT_DIR}/${TESTCASE} ]; then
			echo -e "${BD}${RED}Testcase ${INPUT_DIR}/${TESTCASE} not exist!${ED}"
			exit -1
		fi
	#	echo ${INPUT_DIR}/${TESTCASE}/input.txt 
	#	bash ${INPUT_DIR}/${TESTCASE}/grader_${TESTCASE}.sh
		while read -r line || [[ -n "$line" ]]; 
		do
			COMMANDS=$line
			#echo ${COMMANDS}
			${COMMANDS} >> ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
#			COMMANDS=( $COMMANDS )
#			count=${#COMMANDS[@]}
#			for ((i=0;i<count;i++))
#			do
#				echo ${COMMANDS[i]}
#			done		
		done < "${INPUT_DIR}/${TESTCASE}/input.txt"
	
	 # diff
        	if diff ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt ${INPUT_DIR}/${TESTCASE}/expected.txt >/dev/null 2>&1
        	then
            		echo -e "${GREEN}Testcase $TESTCASE Passed!${ED}"
	    		rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
        	else
            		echo -e "${BD}${RED}""Failed Testcase $TESTCASE! ${ED}"
            		echo -e "${BD}${RED}unmatched: yours${ED}"
            		echo -e "${RED}$(cat ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt)"
	    		echo -e "${BD}${RED}expected${ED}"
            		echo -e "${RED}$(cat ${INPUT_DIR}/${TESTCASE}/expected.txt)${ED}"
	 
	    		rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
 
        	fi
	else
		total=30
		pass=0
		for i in $(seq 1 30)
		do
			TESTCASE=${TEST}${i}
			while read -r line || [[ -n "$line" ]]; 
			do
				COMMANDS=$line
				#echo ${COMMANDS}
				${COMMANDS} >> ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
			done < "${INPUT_DIR}/${TESTCASE}/input.txt"
	
	 # diff
        		if diff ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt ${INPUT_DIR}/${TESTCASE}/expected.txt >/dev/null 2>&1
       			then
	         		pass=`expr ${pass} + 1`	
				rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
        		else
            			echo -e "${BD}${RED}""Failed Testcase $TESTCASE! ${ED}" 
	    			rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
        		fi
	        done
	        echo ""
        	MARK=`expr 3 \* ${pass}`
        	echo -e "${GREEN}[Result] ${pass}/${total} test cases passed${ED}"
        	echo -e "${GREEN}[Mark] $EXCUTABLE: ${MARK} ${ED}"
       		echo ""
			
	fi
}


function performance {

if [ -n "$1" ]
then
  TESTCASE=$1
    if [ "$TESTCASE" == "P1" ]
    then
  
#	TESTCASE="P1"
	if [ ! -d ${INPUT_DIR}/${TESTCASE} ]; then
	echo -e "${BD}${RED}Testcase ${INPUT_DIR}/${TESTCASE} not exist!${ED}"
	exit -1
	fi
	
	before=`ls -lR ${VFS_DIR} | grep -v '^d' | awk '{total += $5} END {print total}'`
	while read -r line || [[ -n "$line" ]]; 
	do
		COMMANDS=$line
		${COMMANDS} >> ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
	done < "${INPUT_DIR}/${TESTCASE}/input.txt"
	 # diff
        if diff ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt ${INPUT_DIR}/${TESTCASE}/expected.txt >/dev/null 2>&1
       	then
		after=`ls -lR ${VFS_DIR} | grep -v '^d' | awk '{total += $5} END {print total}'`	
		#size=$(< ${INPUT_DIR}/${TESTCASE}/size.txt)
		FS=$'\n' read -d '' -r -a lines < ${INPUT_DIR}/${TESTCASE}/size.txt
		allsize=${lines[0]}
		demosize=${lines[1]}
		vfssize=`expr ${after} \- ${before}`
		echo -e "${BD}${YELLOE}Testcase: ${TESTCASE}${ED}"
		echo -e "${BD}${RED}Total file size is:  ${allsize}${ED}"
		echo -e "${BD}${RED}Space TA-demo use: ${demosize}${ED}"
		echo -e "${BD}${RED}Space you use: ${vfssize}${ED}"
		if [ "$vfssize" -lt "$demosize"  ]
		then
			echo -e "${BD}${GREEN}Congrats! You get the 10 bonus marks!${ED}"
		fi
		rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
       	else
            	echo -e "${BD}${RED}""Failed Testcase $TESTCASE! ${ED}" 
            	echo -e "${BD}${RED}unmatched: yours${ED}"
            	echo -e "${RED}$(cat ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt)"
	    	echo -e "${BD}${RED}expected${ED}"
            	echo -e "${RED}$(cat ${INPUT_DIR}/${TESTCASE}/expected.txt)${ED}"
	    	rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
	fi
    elif [ "$TESTCASE" == "P2" ]
    then
#	TESTCASE="P2"

	if [ ! -d ${INPUT_DIR}/${TESTCASE} ]; 
	then
		echo -e "${BD}${RED}Testcase ${INPUT_DIR}/${TESTCASE} not exist!${ED}"
		exit -1
	fi

	if [ -f ${INPUT_DIR}/time.txt ]; 
	then
		rm ${INPUT_DIR}/time.txt
	fi


	#check correctness
	while read -r line || [[ -n "$line" ]]; 
	do
		COMMANDS=$line
		${COMMANDS} >> ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
	done < "${INPUT_DIR}/${TESTCASE}/input.txt"
	 # diff
        if diff ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt ${INPUT_DIR}/${TESTCASE}/expected.txt >/dev/null 2>&1
       	then
		#get the total time
		yourtime="$(awk '{ yourtime += $1 } END { print yourtime }' ${INPUT_DIR}/time.txt)"
		#get TA-demo time
		if [ -f ${INPUT_DIR}/time.txt ]; 
		then
			rm ${INPUT_DIR}/time.txt
		fi
	
		while read -r line || [[ -n "$line" ]]; 
		do
			COMMANDS=$line
			${COMMANDS} >/dev/null 2>&1
		done < "${INPUT_DIR}/${TESTCASE}/input_demo.txt"
		demotime="$(awk '{ demotime += $1 } END { print demotime }' ${INPUT_DIR}/time.txt)"
		echo -e "${BD}${YELLOE}Testcase: ${TESTCASE}${ED}"
#		echo -e "${BD}${RED}The time TA-demo use:"
#		printf '%.3f\n' "$demotime"
#		echo -e "${BD}${RED}The time you use:"
#		printf '%.3f\n' "$yourtime" 
		#compare
		min=0
		if (( $(echo "$yourtime < $demotime" |bc -l) ));
		then
			echo -e "${BD}${GREEN}Congrats! You get the 10 bonus marks! ${ED}"
		else
			echo -e "${BD}${RED}Fail!${ED}"
		fi
		rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
       	else
		echo -e "${BD}${YELLOE}Testcase: ${TESTCASE}${ED}"
            	echo -e "${BD}${RED}""Failed Testcase $TESTCASE! ${ED}" 
            	echo -e "${BD}${RED}unmatched: yours${ED}"
            	echo -e "${RED}$(cat ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt)"
	    	echo -e "${BD}${RED}expected${ED}"
            	echo -e "${RED}$(cat ${INPUT_DIR}/${TESTCASE}/expected.txt)${ED}"
	    	rm  ${OUTPUT_DIR}/${TESTCASE}/output_${TESTCASE}.txt
	fi
    else
	echo -e "${BD}${RED}Non-exist testcases!${ED}"
    fi
else
    echo -e "${BD}${RED}Please specify testcase P1 or P2${ED}"

fi
}


# echo $#
main $1 $2 $3 $4 $5
