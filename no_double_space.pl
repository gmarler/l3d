#!/usr/bin/perl -w
use strict;

my $spacing=0;
while (<>) {
  if($spacing) {
    if( /^[ 	\r\n]*$/ ) {
      next;
    } else {
      $spacing = 0;
    }
  }
  if( /^[ 	\r\n]*$/ ) {
      print;
      $spacing = 1;
      next;
  }
  print;
}
