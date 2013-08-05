package l3dGen;
use strict;
use Portals::World;

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

  #########################################################################
  # create the world file

  # texture images

  # assign a numerical id to each texture image, and print them in this
  # order; they will then be read back in in this order at runtime
  my $ikey;
  my $tex_image_num;
  $tex_image_num = 0;
  for $ikey ( sort keys %{ $self->get_world()->get_all_texture_imagenums() }) {
    $self->get_world()->set_texture_imagenum($ikey, $tex_image_num);

    $tex_image_num++;
  }
  print $tex_image_num, " TEXTURES\n";
  for $ikey ( sort keys %{ $self->get_world()->get_all_texture_imagenums() }) {
    print $ikey, "\n";
  }

  # textures (orientations)

  my %tex_name_to_num = {};

  my $total_actors_and_sectors=0;
  for my $aSector_key( keys % { $self->get_world()->get_all_sectors() } ) {
    # +1 for this sector
    $total_actors_and_sectors++;

    # add in this sectors objects too
    $aSector = $self->get_world()->get_sector($aSector_key);
    for my $dummy2 ( keys % { $aSector->get_all_actors() } ) {
      $total_actors_and_sectors++;
    }
  }
  print "$total_actors_and_sectors SECTORS_AND_ACTORS\n";

  # sector info

  for $aSector_key ( sort keys % { $self->get_world()->get_all_sectors() } ) {
    $aSector = $self->get_world()->get_sector($aSector_key);

    print "SECTOR ", $aSector->name();
    if($aSector->has_geometry()) {
      printf(" %d %d %d %d %d %d ",
        $aSector->node_min_x() - $self->get_world()->get_minx(),
        $aSector->node_min_y() - $self->get_world()->get_miny(),
        $aSector->node_min_z() - $self->get_world()->get_minz(),
        $aSector->node_max_x() - $self->get_world()->get_minx(),
        $aSector->node_max_y() - $self->get_world()->get_miny(),
        $aSector->node_max_z() - $self->get_world()->get_minz());
    }
    print "\n";

    # vertices, facets
    my $num_vertices = 0;
    for my $v ( keys % { $aSector->get_all_vertices() } ) { $num_vertices++; }
    print $num_vertices, " ";
    my $num_facets = 0;
    for my $f ( keys %{$aSector->get_all_facets()} ) { $num_facets++; }
    my $num_portals = 0;
    for my $p ( keys %{$aSector->get_all_portals()} ) { $num_portals++; }
    print $num_facets + $num_portals;
    print "\n";

    # vertices 

    my $vertex_key;
    my $aVertex;
    for $vertex_key ( sort {$a<=>$b} (keys %{$aSector->get_all_vertices()}) ) {
      $aVertex = $aSector->get_vertex($vertex_key);
      print $vertex_key , " ";
      print $aVertex->x() , " ";
      print $aVertex->y() , " ";
      print $aVertex->z() , "\n";
    }

    # facets 

    my $facet_key;
    my $aFacet;

    for $facet_key ( keys %{$aSector->get_all_facets()} ) {
      $aFacet = $aSector->get_facet($facet_key);

      if($aFacet->get_is_invisible()) { next; }

      if ((length($aFacet->get_tex_id) == 0) 
      || ( ! exists $self->get_world()->get_all_textures->
                  {$aFacet->get_tex_id()} ) )
      # could be falsely linked to a nonexistent texture id
      {
        print "GEOMPOLY ";
        print $aFacet->get_vertex_index_count()+1;
        for(my $i=0; $i<=$aFacet->get_vertex_index_count(); $i++) {
          print " " , $aFacet->get_vertex_index($i);
        }
        print "\n";
      }

      else {

       my $aTex = $self->get_world()->get_texture
         (sprintf("%s",$aFacet->get_tex_id()));

        print "GEOMPOLY_TEX "; # ,$aTex->get_tex_id()," ";
        print $aFacet->get_vertex_index_count()+1;
        for(my $i=0; $i<=$aFacet->get_vertex_index_count(); $i++) {
          print " " , $aFacet->get_vertex_index($i);
        }
        # texture image number
        # print " ", $tex_name_to_num{$aTex->get_tex_id()};
        print " ", $self->get_world()->get_texture_imagenum
                     ($aTex->get_image());
        # texture orientation
        printf(" %f %f %f ",
          $aTex->get_origin()->x(),
          $aTex->get_origin()->y(),
          $aTex->get_origin()->z());
        printf(" %f %f %f ",
          $aTex->get_u_vec()->x(),
          $aTex->get_u_vec()->y(),
          $aTex->get_u_vec()->z());
        printf(" %f %f %f ",
          $aTex->get_v_vec()->x(),
          $aTex->get_v_vec()->y(),
          $aTex->get_v_vec()->z());
        print "\n";
      }

    }

    # portals

    my $portal_key;
    my $aPortal;

    for $portal_key ( keys %{$aSector->get_all_portals()} ) {
      $aPortal = $aSector->get_portal($portal_key);
      print "PORTALPOLY ";
      print $aPortal->get_vertex_index_count()+1;
      for(my $i=0; $i<=$aPortal->get_vertex_index_count(); $i++) {
        print " " , $aPortal->get_vertex_index($i);
      }
      print " ",$aPortal->get_target_sector()->name();
      print "\n";
    }

    # actors within this sector

    my $actor_key;
    for $actor_key ( keys %{$aSector->get_all_actors()} ) {
      my $anActor;
      $anActor = $aSector->get_actor($actor_key);
      if ($anActor->is_camera()) {
        print "CAMERA ", $anActor->name(), " ";
        print "no_plugin ";
        print $anActor->position(), " ";
        print $anActor->orientation(), " ";
      } else {
        print "ACTOR ", $anActor->name(), " ";
        print $anActor->pluginfile(), " ";
        print $anActor->position(), " ";
        print $anActor->orientation(), " ";
        print $anActor->meshfile(), " ";
        print $anActor->texturefile(), " ";
        print $anActor->uvfile(), " ";
      }
      print "\n";
    }
  }
}
