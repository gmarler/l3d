package VidscOctreeParser;
use strict;
use Portals::World;
use POSIX;

my $debug = 1;
my $debug_str = "DEBUG-----------------";
my $portal_flag = hex '0x800000';

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_world}   = World->new();
    $self->{_nodes}   = {};

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

sub get_node {
    my $self = shift;
    my $x=shift;
    my $y=shift;
    my $z=shift;

    return $self->{_nodes}{$x}{$y}{$z};
}

sub set_node {
    my $self = shift;
    my $x=shift;
    my $y=shift;
    my $z=shift;
    my $sector=shift;

    $self->{_nodes}{$x}{$y}{$z} = $sector;
}

sub parse {

  my $self = shift;
  my @files = @_;

  for (my $file_idx=0; $file_idx <= $#files; $file_idx++) {

    my $sector = Sector->new();
    $sector->name(sprintf("S%d",$file_idx));
    $sector->has_geometry(1);
    $self->get_world()->add_sector($sector->name(), $sector);

    my %sectors;
    undef %sectors;

    open (FILE,$files[$file_idx]);
    if ($debug) { print "$debug_str $files[$file_idx] opened\n"; }
  
    my $id = <FILE>;
    my $vertex_count = <FILE>; # no. of vertices

    # read in vertex list from file

    for (my $i=0; $i<$vertex_count; $i++) {
      my $vertex = Vertex->new();
      $sector->add_vertex( sprintf("%d",$i), $vertex );
      my $line = <FILE>;

      my @list = split(' ',$line);
      $vertex->x($list[0]);
      $vertex->y($list[1]);
      $vertex->z($list[2]);

      # figure out min, max coordinates among all vertices

      my $node_x = floor($vertex->x() / $self->get_world()->get_nodesizex());
      my $node_y = floor($vertex->y() / $self->get_world()->get_nodesizey());
      my $node_z = floor($vertex->z() / $self->get_world()->get_nodesizez());

      # local min/max (for the mesh)
      if($node_x < $sector->node_min_x()) { $sector->node_min_x($node_x); }
      if($node_x > $sector->node_max_x()) { $sector->node_max_x($node_x); }
      if($node_y < $sector->node_min_y()) { $sector->node_min_y($node_y); }
      if($node_y > $sector->node_max_y()) { $sector->node_max_y($node_y); }
      if($node_z < $sector->node_min_z()) { $sector->node_min_z($node_z); }
      if($node_z > $sector->node_max_z()) { $sector->node_max_z($node_z); }

      # global min/max (across all meshes, bounding box for world)

      if($node_x < $self->get_world()->get_minx())
      { $self->get_world()->set_minx($node_x); }
      if($node_x > $self->get_world()->get_maxx())
      { $self->get_world()->set_maxx($node_x); }
      if($node_y < $self->get_world()->get_miny())
      { $self->get_world()->set_miny($node_y); }
      if($node_y > $self->get_world()->get_maxy())
      { $self->get_world()->set_maxy($node_y); }
      if($node_z < $self->get_world()->get_minz()) 
      { $self->get_world()->set_minz($node_z); }
      if($node_z > $self->get_world()->get_maxz())
      { $self->get_world()->set_maxz($node_z); }
    }

    if($debug) {
      printf("mesh stretches from node %d %d %d to %d %d %d\n",
             $sector->node_min_x(),
             $sector->node_min_y(),
             $sector->node_min_z(),
             $sector->node_max_x(),
             $sector->node_max_y(),
             $sector->node_max_z());
    }

    # create nodes occupied by the current local geometry
    # (more precisely, "occupied by the bounding box of the local geometry")

    my $node_i, my $node_j, my $node_k;
    for($node_i=$sector->node_min_x();
        $node_i<=$sector->node_max_x();
        $node_i++)
    {
      for($node_j=$sector->node_min_y();
          $node_j<=$sector->node_max_y();
          $node_j++)
      {
        for($node_k=$sector->node_min_z();
            $node_k<=$sector->node_max_z();
            $node_k++)
        {
          if ($self->get_node($node_i,$node_j,$node_k)) {
          } else {
            my $new_s;
            $self->set_node($node_i,$node_j,$node_k,$new_s=Sector->new());
            $new_s->name(sprintf("N%03d%03d%03d",$node_i,$node_j,$node_k));
            if($debug) {
              printf("CREATING NODE %d %d %d\n",
                $node_i, $node_j, $node_k);
              if ($self->get_node($node_i,$node_j,$node_k)) {
                printf("create ok\n");
              }
#              $self->get_world()->add_sector($new_s->name(), $new_s);
            }
          }
        }
      }
    }

    # read in facet definitions from file

    my $facet_number = 0;
    while ( <FILE> ) {
      my $facet = Facet->new($sector);
      $sector->add_facet( sprintf("%d",$facet_number), $facet );

      my @list = split(' ');
      my $color = hex $list[@list-1] ;

      my $red = $color & hex '0x0000ff';
      my $grn = ($color & hex '0x00ff00' ) >> 8;
      my $blu = ($color & hex '0xff0000' ) >> 16;
      my $red_dec = $red / 255.0 + 0.005; # 2 sig. digits, round up if 3rd >=5
      my $grn_dec = $grn / 255.0 + 0.005;
      my $blu_dec = $blu / 255.0 + 0.005;
      my $red_hi = int($red_dec * 10.0);
      my $red_lo = int(($red_dec - $red_hi/10.0)*100.0);
      my $grn_hi = int($grn_dec * 10.0);
      my $grn_lo = int(($grn_dec - $grn_hi/10.0)*100.0);
      my $blu_hi = int($blu_dec * 10.0);
      my $blu_lo = int(($blu_dec - $blu_hi/10.0)*100.0);

      my $tex_id = $red_lo * 10000.0 +
                   $grn_hi * 1000.0 +
                   $grn_lo * 100.0 +
                   $blu_hi * 10.0 +
                   $blu_lo * 1.0;

      pop @list; # pop color off list
      shift @list; # don't care about vertex index count

      for(my $i=0; $i<=$#list; $i++) {
        $facet->add_vertex_index( $list[$i] );
      }

      if ($red_hi == 1.0) {
        if ($facet->get_vertex_index_count() != 2) {
          printf("Non-triangle marked as geometry and tex-def-tri. ID: %d.\n",
                 $tex_id);
        } else {
          printf("Geometry and tex-def-tri found. ID: %d\n", $tex_id);
          $facet->set_tex_id($tex_id);
  
          my $tex = Texture->new();
          $tex->set_tex_id($tex_id);
          $tex->set_origin($facet->get_tex_origin());
          $tex->set_u_vec($facet->get_tex_u_vec());
          $tex->set_v_vec($facet->get_tex_v_vec());
          $self->get_world()->add_texture(sprintf("%d",$tex_id), $tex);
        }
      } elsif ($red_hi == 2.0) {
        if ($facet->get_vertex_index_count() != 2) {
          printf("Non-triangle marked as pure tex-def-tri. ID: %d.\n",
                 $tex_id);
        } else {
          printf("Pure tex-def-tri found. ID: %d\n", $tex_id);
          $facet->set_tex_id($tex_id);
  
          my $tex = Texture->new();
          $tex->set_id($tex_id);
          $tex->set_origin($facet->get_tex_origin());
          $tex->set_u_vec($facet->get_tex_u_vec());
          $tex->set_v_vec($facet->get_tex_v_vec());
          $self->get_world()->add_texture($tex);
        }
      } elsif ( ($red_hi == 0.0)  && ($tex_id != 0) ) {
        printf("Geometry poly linked to tex ID: %d\n", $tex_id);
        $facet->set_tex_id($tex_id);
      }

      $facet_number++;
    }
  }
  # split polygons to occupied nodes (not yet implemented, or rather
  # in c++ implemented during level loading)

  # don't bother creating any nodes; all the populated ones will
  # all have been created during mesh reading anyway

  my $i,my $j, my $k;

  # however we do want to RENAME the nodes so that no negative
  # node names exist. we can only do this AFTER all nodes have been
  # created, since only then do we know the global min node coords.

#   # create all nodes
#   # also add all nodes to the world, **IN PROPER ORDER** to be read in
#   # by program (thus we shouldn't create nodes earlier, since these
#   # would be inserted in random order into the world sector list)
  for($k=$self->get_world()->get_minz();
      $k<=$self->get_world()->get_maxz();
      $k++)
  {
    for($j=$self->get_world()->get_miny();
        $j<=$self->get_world()->get_maxy();
        $j++)
    {
      for($i=$self->get_world()->get_minx();
          $i<=$self->get_world()->get_maxx();
          $i++)
      {
        my $sector;
        if($sector = $self->get_node($i,$j,$k)) {
          if($debug) { printf("fill: node %d %d %d exists\n",$i,$j,$k); }

          # rename sector so no negative #s appear
          $sector->name(sprintf("N%03d%03d%03d",
            $i-$self->get_world()->get_minx(),
            $j-$self->get_world()->get_miny(),
            $k-$self->get_world()->get_minz()));

          $self->get_world()->add_sector($sector->name(), $sector);
        } else {
#           $self->set_node($i,$j,$k,$sector = Sector->new());
#           $sector->name(sprintf("N%03d%03d%03d",
#             $i-$self->get_world()->get_minx(),
#             $j-$self->get_world()->get_miny(),
#             $k-$self->get_world()->get_minz()));
#           if($debug) { printf("fill: empty node %d %d %d being created\n",
#                                $i,$j,$k); }
        }
      }
    }
  }

  # create portals between nodes
  for($i=$self->get_world()->get_minx();
      $i<=$self->get_world()->get_maxx();
      $i++)
  {
    for($j=$self->get_world()->get_miny();
        $j<=$self->get_world()->get_maxy();
        $j++)
    {
      for($k=$self->get_world()->get_minz();
          $k<=$self->get_world()->get_maxz();
          $k++)
      {
        my $sector;
        $sector = $self->get_node($i,$j,$k);
        if($sector) {
          if($debug) { printf("node %d %d %d: %s\n",$i,$j,$k,
                              $sector->name()); }
          # create vertex list for this octree node
          # 8 vertices, one for each corner
          my $vtx_num = 0;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez());
          $vtx_num++;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex() +
                        $self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez());
          $vtx_num++;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey() +
                        $self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez());
          $vtx_num++;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex() +
                        $self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey() +
                        $self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez());
          $vtx_num++;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez() +
                        $self->get_world()->get_nodesizez());
          $vtx_num++;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex() +
                        $self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez() +
                        $self->get_world()->get_nodesizez());
          $vtx_num++;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey() +
                        $self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez() +
                        $self->get_world()->get_nodesizez());
          $vtx_num++;
  
          my $vertex = Vertex->new();
          $sector->add_vertex( sprintf("%d",$vtx_num), $vertex );
          $vertex->x($i*$self->get_world()->get_nodesizex() +
                        $self->get_world()->get_nodesizex());
          $vertex->y($j*$self->get_world()->get_nodesizey() +
                        $self->get_world()->get_nodesizey());
          $vertex->z($k*$self->get_world()->get_nodesizez() +
                        $self->get_world()->get_nodesizez());
          $vtx_num++;
          
          # create portals for the sector
          # potentially we create portals linking to nonexistent
          # sectors since we only create sectors occupied by geometry.
          # such "dangling" portals are deleted when we link all
          # portals together in world->link_portals().
  
          my $portal_num = 0;
          if($i!=$self->get_world()->get_minx()) {
            if($debug) { printf("    --> left portal\n"); }
            my $portal = Portal->new($sector);
            $sector->add_portal( sprintf("%d",$portal_num), $portal );
            $portal->add_vertex_index(0);
            $portal->add_vertex_index(2);
            $portal->add_vertex_index(6);
            $portal->add_vertex_index(4);
            $portal->set_target_sector($self->get_node($i-1,$j,$k));
            $portal_num++;
          }
          if($i!=$self->get_world()->get_maxx()) {
            if($debug) { printf("    --> rightportal\n"); }
            my $portal = Portal->new($sector);
            $sector->add_portal( sprintf("%d",$portal_num), $portal );
            $portal->add_vertex_index(1);
            $portal->add_vertex_index(5);
            $portal->add_vertex_index(7);
            $portal->add_vertex_index(3);
            $portal->set_target_sector($self->get_node($i+1,$j,$k));
            $portal_num++;
          }
          if($j!=$self->get_world()->get_miny()) {
            if($debug) { printf("    --> bottom portal\n"); }
            my $portal = Portal->new($sector);
            $sector->add_portal( sprintf("%d",$portal_num), $portal );
            $portal->add_vertex_index(0);
            $portal->add_vertex_index(4);
            $portal->add_vertex_index(5);
            $portal->add_vertex_index(1);
            $portal->set_target_sector($self->get_node($i,$j-1,$k));
            $portal_num++;
          }
          if($j!=$self->get_world()->get_maxy()) {
            if($debug) { printf("    --> top portal\n"); }
            my $portal = Portal->new($sector);
            $sector->add_portal( sprintf("%d",$portal_num), $portal );
            $portal->add_vertex_index(2);
            $portal->add_vertex_index(3);
            $portal->add_vertex_index(7);
            $portal->add_vertex_index(6);
            $portal->set_target_sector($self->get_node($i,$j+1,$k));
            $portal_num++;
          }
          if($k!=$self->get_world()->get_minz()) {
            if($debug) { printf("    --> front portal\n"); }
            my $portal = Portal->new($sector);
            $sector->add_portal( sprintf("%d",$portal_num), $portal );
            $portal->add_vertex_index(0);
            $portal->add_vertex_index(1);
            $portal->add_vertex_index(3);
            $portal->add_vertex_index(2);
            $portal->set_target_sector($self->get_node($i,$j,$k-1));
            $portal_num++;
          }
          if($k!=$self->get_world()->get_maxz()) {
            if($debug) { printf("    --> back portal\n"); }
            my $portal = Portal->new($sector);
            $sector->add_portal( sprintf("%d",$portal_num), $portal );
            $portal->add_vertex_index(4);
            $portal->add_vertex_index(6);
            $portal->add_vertex_index(7);
            $portal->add_vertex_index(5);
            $portal->set_target_sector($self->get_node($i,$j,$k+1));
            $portal_num++;
          }
        }
      }
    }
  }

  # unlike the videoscape portal parser, we don't need to create portals
  # from the holes here

  $self->get_world()->link_portals();
  
}
