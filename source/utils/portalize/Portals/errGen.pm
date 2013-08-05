package errGen;
use strict;
use Portals::World;
use FileHandle;

my $debug = 0;
my $debug_str = "DEBUG-----------------";
my $portal_flag = hex '0x800000';

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_world}   = undef;
    bless ($self, $class);
    return $self;
}

##############################################
## methods to access per-object data        ##
##                                          ##
## With args, they set the value.  Without  ##
## any, they only retrieve it/them.         ##
##############################################

sub get_world {
    my $self = shift;

    return $self->{_world};
}

sub set_world {
    my $self = shift;
    my $w = shift;

    $self->{_world} = $w;
}


sub generate {
  my $self = shift;

  my $aSector_key;
  my $aSector;

  my $unlinked_num;
  my $unlooped_num;

  # error report

  unlink <nonlinked.obj*>;
  unlink <nonlooped.obj*>;
  for $aSector_key ( keys % { $self->get_world()->get_all_sectors() } ) {
    my $portal_key;
    my $aPortal;

    $aSector = $self->get_world()->get_sector($aSector_key);

    if ( keys %{$aSector->get_all_nonlooped_portals} ) {
      print STDERR "Sector ", $aSector_key, " has nonlooped portals.\n";

      # vertices 
  
      my $fname;

      if ($unlooped_num) {
        $fname = sprintf("> nonlooped.obj%d", $unlooped_num++);
      } else {
        $fname = sprintf("> nonlooped.obj");
        $unlooped_num = 1;
      }

      my $fh = FileHandle->new();
      $fh->open($fname);
      print $fh "3DG1\n";

      my $num_vertices = 0;
      for my $v ( keys % { $aSector->get_all_vertices() } ) { $num_vertices++; }
      print $fh $num_vertices,"\n";
      my $vertex_key;
      my $aVertex;
      for $vertex_key ( sort{$a<=>$b} keys %{$aSector->get_all_vertices()} ) {
        $aVertex = $aSector->get_vertex($vertex_key);
        # print "(" , $vertex_key , ")";
        print $fh " " , $aVertex->x();
        print $fh " " , $aVertex->z(); # reverse y and z to conv. lhs<->rhs
        print $fh " " , $aVertex->y();
        print $fh "\n";
      }

      for $portal_key ( keys %{$aSector->get_all_nonlooped_portals()} ) {
        $aPortal = $aSector->get_nonlooped_portal($portal_key);

        # since the portal may have more than 4 sides, and since Blender
        # only handles polygons with max 4 sides, we can't write out the
        # portal as a single polygon; instead we write it out edge by edge
        my $edge;
        for $edge ( $aPortal->get_edge_list() ) {
          print $fh "2 ",$edge->[0], " ", $edge->[1];
          print $fh " 0xFFFFFF\n";
        }
      }

      $fh->close();
    }
    if ( keys %{$aSector->get_all_nonlinked_portals} ) {
      print STDERR "Sector ", $aSector_key, " has nonlinked portals.\n";

      # vertices 

      my $fname;

      if ($unlinked_num) {
        $fname = sprintf("> nonlinked.obj%d", $unlinked_num++);
      } else {
        $fname = sprintf("> nonlinked.obj");
        $unlinked_num = 1;
      }

      my $fh = FileHandle->new();
      $fh->open($fname);
      print $fh "3DG1\n";
  
      my $num_vertices = 0;
      for my $v ( keys % { $aSector->get_all_vertices() } ) { $num_vertices++; }
      print $fh $num_vertices,"\n";
      my $vertex_key;
      my $aVertex;
      for $vertex_key ( sort{$a<=>$b} keys %{$aSector->get_all_vertices()} ) {
        $aVertex = $aSector->get_vertex($vertex_key);
        # print "(" , $vertex_key , ")";
        print $fh " " , $aVertex->x();
        print $fh " " , $aVertex->z(); # reverse y and z to conv. lhs<->rhs
        print $fh " " , $aVertex->y();
        print $fh "\n";
      }

      for $portal_key ( keys %{$aSector->get_all_nonlinked_portals()} ) {
        $aPortal = $aSector->get_nonlinked_portal($portal_key);

        # since the portal may have more than 4 sides, and since Blender
        # only handles polygons with max 4 sides, we can't write out the
        # portal as a single polygon; instead we write it out edge by edge
        my $edge;
        for $edge ( $aPortal->get_edge_list() ) {
          print $fh "2 ",$edge->[0], " ", $edge->[1];
          print $fh " 0xFFFFFF\n";
        }
      }

      $fh->close();

    }
  }
}
