package OctreeNode;
use strict;
use Portals::Facet;
use Portals::Portal;
use Portals::Vertex;
use Portals::Sector;

@ISA = qw(Sector);

############################
## the object constructor ##
############################
sub new {

    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_name}   = undef;
    $self->{_vertices} = {};
    $self->{_facets}  = {}; 
    $self->{_portals}  = {};
    $self->{_nonlooped_portals} = {};
    $self->{_nonlinked_portals} = {};
    $self->{_free_edges}  = [];

    $self->{_portal_top} = undef;
    $self->{_portal_bottom} = undef;
    $self->{_portal_left} = undef;
    $self->{_portal_right} = undef;
    $self->{_portal_front} = undef;
    $self->{_portal_back} = undef;
    $self->{_facet_refs} = {};

    bless ($self, $class);
    return $self;
}

##############################################
## methods to access per-object data        ##
##                                          ##
## With args, they set the value.  Without  ##
## any, they only retrieve it/them.         ##
##############################################

sub name {
    my $self = shift;
    if (@_) { $self->{_name} = shift }

    return $self->{_name};
}

sub get_all_vertices {
    my $self = shift;
    if (@_) { $self->{_vertices} = shift }

    return $self->{_vertices};
}

sub get_vertex {
    my $self = shift;
    my $vertex_index = shift;

    return $self->{_vertices}->{$vertex_index};
}

#not yet implemented:
#sub delete_vertex {
#    my $self = shift;
#}

sub add_vertex {
    my $self = shift;
    my $vertex_index = shift;
    my $vertex = shift;
    $self->{_vertices}->{$vertex_index} = $vertex;
}

sub get_all_facets {
    my $self = shift;

    return $self->{_facets};

    #if (@_) { @{ $self->{_facets} } = @_ }
    #return @{ $self->{_facets} };
}

sub get_facet {
  my $self= shift;
  my $facet_index = shift;
  return $self->{_facets}->{$facet_index};
}

sub add_facet {
  my $self = shift;
  my $facet_index = shift;
  my $facet = shift;

  $self->{_facets}->{$facet_index} = $facet;
}


sub get_all_portals {
    my $self = shift;
    if (@_) { $self->{_portals} = shift }

    return $self->{_portals};
}

sub get_portal {
  my $self= shift;
  my $portal_index = shift;
  return $self->{_portals}->{$portal_index};
}

sub add_portal {
  my $self = shift;
  my $portal_index = shift;
  my $portal = shift;

  $self->{_portals}->{$portal_index} = $portal;
}

sub delete_portal {
  my $self = shift;
  my $portal = shift;

  for my $a_index ( keys %{$self->get_all_portals()} ) {
    if ($self->get_portal($a_index) == $portal) {
      print "Portal deleted.\n";
      delete $self->{_portals}->{$a_index};
    }
  }
}



sub get_free_edges {
    my $self = shift;

    return  $self->{_free_edges} ;
}


sub make_portals_from_holes {
  my $self = shift;

  printf("Making portals from holes:\n");
  $self->_find_free_edges();
  $self->_merge_free_edges_into_portals();
}

sub add_nonlooped_portal {
    my $self = shift;
    my $portal_index = shift;
    my $portal = shift;

    $self->{_nonlooped_portals}{$portal_index} = $portal;
}

sub get_nonlooped_portal {
  my $self= shift;
  my $portal_index = shift;
  return $self->{_nonlooped_portals}->{$portal_index};
}

sub get_all_nonlooped_portals {
    my $self = shift;

    return $self->{_nonlooped_portals};
}

sub add_nonlinked_portal {
    my $self = shift;
    my $portal_index = shift;
    my $portal = shift;

    $self->{_nonlinked_portals}{$portal_index} = $portal;
}

sub get_all_nonlinked_portals {
    my $self = shift;

    return $self->{_nonlinked_portals};
}

sub get_nonlinked_portal {
  my $self= shift;
  my $portal_index = shift;
  return $self->{_nonlinked_portals}->{$portal_index};
}


###################### PRIVATE #######################

sub _find_free_edges {
  my $self = shift;

  my $aSector = $self;
  my $aFacet_key;
  my $aFacet;
  my $aOtherFacet_key;
  my $aOtherFacet;
  my @edge_list;

  print "Scanning for free edges, Sector ", $aSector->name(), "\n";
  for $aFacet_key ( keys % { $aSector->get_all_facets() } ) {
    $aFacet = $aSector->get_facet($aFacet_key);
    print "  Facet ", $aFacet_key, "\n";
    @edge_list = $aFacet->get_edge_list();
    for ( my $i=0; $i<=$#edge_list; $i++) {
      my $edge_taken = undef;
      for $aOtherFacet_key ( keys % { $aSector->get_all_facets() } ) {
        if($aOtherFacet_key != $aFacet_key) {  # for all OTHER facets
          $aOtherFacet = $aSector->get_facet($aOtherFacet_key);
          if($aOtherFacet ->contains_edge($edge_list[$i][0],$edge_list[$i][1]))
          {
            $edge_taken = 1;
          }
        }
      }
      if ( !$edge_taken ) {
        print "    FREE EDGE ", $edge_list[$i][0]," ", $edge_list[$i][1], "\n";
        push @{$self->get_free_edges()},  $edge_list[$i] ;
      }
    }
  }
  print "  Summary of found free edges:\n";
  for ( my $i=0; $i<=$#{$self->get_free_edges()}; $i++) {
    print "  ";
    print ${@{$self->get_free_edges()}[$i]}[0]," ";
    print ${@{$self->get_free_edges()}[$i]}[1],"\n";
  }
}

sub _merge_free_edges_into_portals {
  my $self = shift;
  my $portal;

  # note that we must link the edges from pt1 to pt0 instead of pt0 to pt1
  # because we must reverse the orientation of the edges to make the portal
  # normal point the same direction as the neighboring facets' normals.

  my $portal_num = 0;
  my $unlooped_portal_num = 0;
  while ( ( $#{$self->get_free_edges()} >= 0 ) ) {

    my $error = undef;

    print "    Creating new portal.\n";
    $portal = Portal->new($self);
    $self->add_portal(sprintf("%d",$portal_num), $portal);

    my $e_idx = 0;
    my $idx1 = ${@{$self->get_free_edges()}[$e_idx]}[1];
    my $idx0 = ${@{$self->get_free_edges()}[$e_idx]}[0];

    # add current vertex to vertex list
    $portal->add_vertex_index( $idx1 );
    splice ( @{$self->get_free_edges()} , $e_idx, 1 ); # remove current edge
    print "      Starting vertex $idx1.\n";

    my $first_idx = $idx1;

    while ( ( $idx0 != $first_idx ) && !$error ) {
      # keep going through vertex pairs until you hit the starting vertex again

      # find daisy-chained edge

      my $i;
      my $done = undef;

      for ( $i=0; ($i<=$#{$self->get_free_edges()}) && (!$done); $i++) {
        my $candidate_idx1 = ${@{$self->get_free_edges()}[$i]}[1];
        my $candidate_idx0 = ${@{$self->get_free_edges()}[$i]}[0];
    
        if ( $candidate_idx1 == $idx0 ) {
          $idx1 = $candidate_idx1;
          $idx0 = $candidate_idx0;

          # add current vertex to vertex list
          $portal->add_vertex_index( $idx1 );
          print "      Daisy-chaining vertex $idx1.\n";

          $e_idx = $i;
          # remove current edge
          splice ( @{$self->get_free_edges()} , $e_idx, 1 );
          $done = 1;
        }
      }

      if ( !$done) {
        print "Uh oh, free edges couldn't be joined into a loop; skipping...\n";
        $self->delete_portal($portal);
        $self->add_nonlooped_portal(sprintf("%d",$unlooped_portal_num++),
                                    $portal);
        $done = 1;
        $error = 1; 
      }
    }
    if ( ! $error ) {
      print "    Portal complete.\n";
      $portal_num++;
    }
  }
}


1;  # so the require or use succeeds
