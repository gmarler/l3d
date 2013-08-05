package Vertex;
use strict;

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_x}   = undef;
    $self->{_y} = undef;
    $self->{_z}  = undef; 
    bless ($self, $class);
    return $self;
}

##############################################
## methods to access per-object data        ##
##                                          ##
## With args, they set the value.  Without  ##
## any, they only retrieve it/them.         ##
##############################################

sub x {
    my $self = shift;
    if (@_) { $self->{_x} = shift }

    return $self->{_x};
}

sub y {
    my $self = shift;
    if (@_) { $self->{_y} = shift }

    return $self->{_y};
}

sub z {
    my $self = shift;
    if (@_) { $self->{_z} = shift }

    return $self->{_z};
}

sub dot {
  my $self = shift;
  my $v2 = shift;

  return $self->x * $v2->x +
         $self->y * $v2->y +
         $self->z * $v2->z ;
}

sub dif {
  my $self = shift;
  my $v2 = shift;

  my $vnew = Vertex->new();
  $vnew->x ( $self->x - $v2->x );
  $vnew->y ( $self->y - $v2->y );
  $vnew->z ( $self->z - $v2->z );

  return $vnew;
}

1;  # so the require or use succeeds
