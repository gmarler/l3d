#!/usr/bin/perl -w

use strict;
use FileHandle;

my $file;
for $file ( `find . -name '*.cc' -print -o -name '*.h' -print `) {
  chomp $file;
  print $file, "\n";
  print "==========================================================\n";

  my %files;
  
  my $curfile = FileHandle->new();
  open($curfile, $file);

  while ( <$curfile> ) {
    if ( /^[ 	]*\/\/sbegin/) {
      my $name;
      my @parts = split;
      $name = join(".", $file, "SN", $parts[1]);
      print "opening " , $name, "\n";
      $files{$name} = FileHandle->new();
      open($files{$name}, join("", ">", $name));
    } elsif ( /^[ 	]*\/\/send/) {
      my $name;
      my @parts = split;
      $name = join(".", $file, "SN", $parts[1]);
      print "closing ", $name, "\n";
      close($files{$name});
      delete $files{$name};
    } else {
      for my $i ( keys %files ) {
        print { $files{$i} } $_;
      }
    }
  }
  
  for my $i ( keys %files ) {
    print "closing unclosed ", $i, "\n";
    close $files{$i};
  }


  close($curfile);
}
