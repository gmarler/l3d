package Facet;
use strict;

my $debug = undef;

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;

    my $parent = shift;

    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_is_invisible}   = 0;
    $self->{_tex_id}   = "";
    $self->{_vertex_indices}   = [];
    $self->{_parent_sector}   = $parent;
    bless ($self, $class);
    return $self;
}

##############################################
## methods to access per-object data        ##
##                                          ##
## With args, they set the value.  Without  ##
## any, they only retrieve it/them.         ##
##############################################

sub get_is_invisible {
  my $self = shift;
  return $self->{_is_invisible};
}

sub set_is_invisible {
  my $self = shift;
  my $val = shift;
  $self->{_is_invisible} = $val;
}

sub get_tex_id {
  my $self = shift;
  return $self->{_tex_id};
}

sub set_tex_id {
  my $self = shift;
  my $id = shift;
  $self->{_tex_id} = $id;
}

sub get_parent_sector {
  my $self = shift;
  return $self->{_parent_sector};
}

sub set_parent_sector {
  my $self = shift;
  my $parent = shift;
  $self->{_parent_sector} = $parent;
}

sub get_vertex_index {
    my $self = shift;
    my $position = shift;

    return $self->{_vertex_indices}[$position];
}

sub get_vertex_index_count {
    my $self = shift;
    return $#{$self->{_vertex_indices} };
}

sub add_vertex_index {
    my $self = shift;
    my $index = shift;

    push @{$self->{_vertex_indices}} ,  $index;
}

#sub get_vertex_index_list {
#    my $self = shift;
#
#    return @{ $self->{_vertex_indices}};
#}

sub get_edge_list {
  my $self = shift;

  my $i;
  my @edge_list;
  for($i=0; $i <= $self->get_vertex_index_count(); $i++) {
    my $j;
    $edge_list[$i] = [ $self->get_vertex_index($i) ];
    $j = $i + 1;
    if($j > $self->get_vertex_index_count()) {
      $j = 0;
    }
    push @{$edge_list[$i]} , $self->get_vertex_index($j);
  }
  return @edge_list;
}

sub contains_edge {
  my $self = shift;
  my $idx0 = shift;
  my $idx1 = shift;

  my $found = undef;

  my @edge_list = $self->get_edge_list();
  for( my $i=0; ( $i <= $#edge_list ) && ( !$found ); $i++ ) {
    if ( ( ( $edge_list[$i][0] == $idx0 ) && ( $edge_list[$i][1] == $idx1 ) )
       ||( ( $edge_list[$i][0] == $idx1 ) && ( $edge_list[$i][1] == $idx0 ) ) )
    {
      $found = 1;
    }
  }

  return $found;
}

sub coincident_with_facet {
  my $self = shift;
  my $epsilon = 1.001;

  my $other_facet = shift;
  my $result = 1;

  for ( my $i = 0; ($i <= $self->get_vertex_index_count()) && $result; $i++ ) {
    my $vertex_found_in_other = undef;
    for ( my $other_i = 0;
          ($other_i <= $other_facet->get_vertex_index_count())
          && !$vertex_found_in_other;
          $other_i++)
    {
      my $parent = $self->get_parent_sector();
      my $other_parent = $other_facet->get_parent_sector();
      if(
      (abs($parent->get_vertex($self->get_vertex_index($i)) -> x() -
           $other_parent->get_vertex
                              ($other_facet->get_vertex_index($other_i))->x())
           < $epsilon)
      &&(abs($parent->get_vertex($self->get_vertex_index($i)) -> y() -
          $other_parent->get_vertex
                               ($other_facet->get_vertex_index($other_i))->y())
           < $epsilon)
      &&(abs($parent->get_vertex($self->get_vertex_index($i)) -> z() -
          $other_parent->get_vertex
                               ($other_facet->get_vertex_index($other_i))->z())
           < $epsilon)
      )
      {
        $vertex_found_in_other = 1;
      }
    }
    if ( ! $vertex_found_in_other ) {
      $result = undef;
    }
  }

  return $result;
}


sub get_tex_ouv() {
  my $self = shift;

  my $v1, my $v2, my $v3;
  my $epsilon = 0.001;

  $v1 = Vertex->new();
  $v2 = Vertex->new();
  $v3 = Vertex->new();

  $v1->x ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(0)) -> x );
  $v1->y ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(0)) -> y() );
  $v1->z ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(0)) -> z() );
  $v2->x ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(1)) -> x() );
  $v2->y ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(1)) -> y() );
  $v2->z ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(1)) -> z() );
  $v3->x ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(2)) -> x() );
  $v3->y ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(2)) -> y() );
  $v3->z ( $self->get_parent_sector()->get_vertex
             ($self->get_vertex_index(2)) -> z() );

  # try origin at v1

  if ( $v2->dif($v1)->dot($v3->dif($v1)) < $epsilon) {
    if ($debug) { print "origin v1\n"; }
    return { o => $v1,
             v => $v3,
             u => $v2 };
             
  }

  # try origin at v2

  if ( $v1->dif($v2)->dot($v3->dif($v2)) < $epsilon) {
    if ( $debug )  { print "origin v2\n"; }
    return { o => $v2,
             v => $v1,
             u => $v3 };
  }

  # try origin at v3

  if ( $v1->dif($v3)->dot($v2->dif($v3)) < $epsilon) {
    if ( $debug ) { print "origin v3\n"; }
    return { o => $v3,
             v => $v2,
             u => $v1 };
  }
 
  else {
    if ( $debug ) {  print "Hm... no origin found!\n" };
    print "WARNING: Invalid texture definition triangle found";
    print " (orientation could not be determined).\n";
  }

}

sub get_tex_origin() {
  my $self = shift;

  return $self->get_tex_ouv()->{'o'};

}

sub get_tex_u_vec() {
  my $self = shift;
  return $self->get_tex_ouv()->{'u'};
}

sub get_tex_v_vec() {
  my $self = shift;
  return $self->get_tex_ouv()->{'v'};
}

1;  # so the require or use succeeds
