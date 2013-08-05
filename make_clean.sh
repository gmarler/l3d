#!/bin/sh
# see i told you it was gone
cvt_program=/usr/local/corel_wp/shbin10/cvt
d=/home2/nlingfx/book_text/current/wordware
clean_dir=source_clean

rm -rf $d/$clean_dir
mkdir $d/$clean_dir

for i in `find source -type d -print`
do
  b=`echo $i | sed -e 's/source\///' | gawk '{print "'$clean_dir'/" $1}'`;
  echo mkdir $d/$b
  mkdir $d/$b
done

for i in `find source -name '*.cc' -print -o -name '*.h' -print -o -name '*SN.*' -print -o -name '*.pl' -print -o -name '*.pm' -print`
do
  b=`echo $i | sed -e 's/source\///' | gawk '{print "'$clean_dir'/" $1}'`;

  echo $i " -> " $d/$b;
  # explanation of ugly sed strings below
  #
  # 1,2,3 elim all lines with just //comments that aren't //-special comments 
  # 4     elim asserts (MUST BE ON ONE LINE)
  # 5,6   escape curly braces { } into \{ and \}
  # 7     truncate lines with just spaces
  # 8     escape \ at the end of a line to be \\
  # 9     perl script to merge multiple empty lines to just one 
  # 10    replace beginning of each line with an italic space, then
  #       immediately turning off italics again
  #       (needed for proper WP8 <> MS-word export, otherwise font size
  #       is not exported)
  # 11    closing braces
  #
  # RTF file is then converted with a WP8 utility into native WP8 format
  # so that we don't get an interactive query (convert from file type?)
  # during batch-mode master-document-expansion of the code snippets

  ( cat template.rtf ; ( cat $i  | sed -e '/^[	 ]*\/\/[^\-].*/d' -e '/^[	 ]*\/\/ *$/d' -e 's/\/\/[^\-].*$//' -e 's/^[	 ]*assert.*;[	 ]*$//'  -e 's/{/\\{/g' -e 's/}/\\}/g' -e 's/[ 	]*$//' -e 's/\\$/\\\\/' | perl no_double_space.pl | sed -e 's/^/{\\i }{ /' | sed -e 's/$/\\par}/') ; echo "}}" ) > $d/$b

  # call to $cvt_program (filtrix) commented out, since it appears that
  # it may munge the converted file with random characters!!!!!!
  # (damn password protection)

  # $cvt_program $d/$b $d/$b.converted
  # mv $d/$b.converted $d/$b
done
