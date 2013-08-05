#!/usr/bin/perl

use FindBin;
use lib "$FindBin::Bin";

use Portals::VidscOctreeParser;
use Portals::l3dGen;
use Portals::errGen;
use strict;

my $parser = VidscOctreeParser->new();
$parser->parse(<@ARGV>);

my $g = errGen->new();
$g->set_world($parser->get_world());
$g->generate();

my $g = l3dGen->new();
$g->set_world($parser->get_world());
$g->generate();
