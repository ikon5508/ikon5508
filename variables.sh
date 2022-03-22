OS=$(uname)

echo $OS
read -p "Enter a string: " str1 
if [ $str1 = 'linux' ]; then
	echo "linux"
elif [ $str1 = 'unix' ]; then
	echo "unix"
else
	echo "Neither linux nor unix"
fi
