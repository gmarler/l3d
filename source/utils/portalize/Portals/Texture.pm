package Texture;
use strict;

use Portals::Vertex;

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;

    my $parent = shift;

    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_tex_id}   = "";
    $self->{_u_vec}   = Vertex->new();
    $self->{_v_vec}   = Vertex->new();
    $self->{_origin}   = Vertex->new();
    $self->{_image}   = "";
    bless ($self, $class);
    return $self;
}

##############################################
## methods to access per-object data        ##
##                                          ##
## With args, they set the value.  Without  ##
## any, they only retrieve it/them.         ##
##############################################

sub get_tex_id {
  my $self = shift;
  return $self->{_tex_id};
}

sub set_tex_id {
  my $self = shift;
  my $id = shift;
  $self->{_tex_id} = $id;
}

sub get_u_vec {
  my $self = shift;
  return $self->{_u_vec};
}

sub set_u_vec {
  my $self = shift;
  my $vec = shift;
  $self->{_u_vec} = $vec;
}

sub get_v_vec {
  my $self = shift;
  return $self->{_v_vec};
}

sub set_v_vec {
  my $self = shift;
  my $vec = shift;
  $self->{_v_vec} = $vec;
}

sub get_origin {
  my $self = shift;
  return $self->{_origin};
}

sub set_origin {
  my $self = shift;
  my $o = shift;
  $self->{_origin} = $o;
}

sub get_image {
  my $self = shift;
  return $self->{_image};
}

sub set_image {
  my $self = shift;
  my $i = shift;
  $self->{_image} = $i;
}

1;  # so the require or use succeeds
