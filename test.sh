
make debug > /dev/null
rm ../test/*.o ../test/*.i

for file in `find ../test/ -name '*.tst' | sort`; do
	./ahed -c -i $file -o $file.o -l $file.log
	./ahed -x -i $file.o -o $file.i -l $file.log.o

	diff -q $file $file.i > /dev/null 2>&1 && diff -q $file.log  $file.log.o > /dev/null 2>&1

	if [ $? -eq 0 ] ; then
		res=OK
	else
		res=FAIL
	fi

	printf "%30s %4s\n" $file $res

	if [[ "$res" = "FAIL" ]] ; then
		echo "xxd? y/N"
		read 

		if [[ "$REPLY" = "y" ]] ; then
			xxd -b $file.o
			xxd -b $file
			xxd -b $file.i
		fi
		echo "========================"
		diff $file.log cat $file.log.o 2>/dev/null
	fi
		
done
