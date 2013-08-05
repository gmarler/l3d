package VidscParser;

# defines variable $Bin, which is dir from which script was invoked
use FindBin;
use lib "$FindBin::Bin";

use strict;
use Portals::World;
use Portals::Actor;
use FileHandle;

my $debug = 0;
my $debug_str = "DEBUG-----------------";
my $portal_flag = hex '0x800000';
my $id_extraction_program = "$FindBin::Bin/../vidinfo/vidinfo";
my $attrib_filename_format_str = "attrib%d.dat";
my $attrib_dir = "";

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_world}   = World->new();
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

sub parse {

  my $self = shift;
  my @files = @_;

  my @actors;

  for (my $file_idx=0; $file_idx <= $#files; $file_idx++) {
    my @mesh_properties;
    my $mesh_type;
    my $mesh_name;
    my $cmd = join(" ", $id_extraction_program, $files[$file_idx]);   
    my @extracted_info = `$cmd`;
    my $extracted_id = $extracted_info[0];
    if($extracted_id) {
      my $attrib_file = sprintf($attrib_filename_format_str, $extracted_id);

      my @attrib_dir_tokens = split('/', $files[$file_idx]);
      pop @attrib_dir_tokens; # get path of current videoscape file
      $attrib_dir = join("/", @attrib_dir_tokens);
      if( length($attrib_dir) ) {
        $attrib_file = join("/",$attrib_dir, $attrib_file);
      } else {
        $attrib_file = $attrib_file;
      }
      my $fh = FileHandle->new();
      $fh->open($attrib_file);
      while(<$fh>) {
        push @mesh_properties, $_;
      }
      $fh->close();

      my @mesh_types = grep( /^TYPE /  , @mesh_properties);
      $mesh_type = $mesh_types[0];
      my @mesh_tokens = split(' ', $mesh_type);
      $mesh_type = $mesh_tokens[1];

      my @mesh_names = grep( /^NAME /  , @mesh_properties);
      $mesh_name = $mesh_names[0];
      my @meshname_tokens = split(' ', $mesh_name);
      $mesh_name= $meshname_tokens[1];
    } else {
      $mesh_type = undef;
      $mesh_name = undef;
    }

    if (lc($mesh_type) eq "actor") {
      my $actor = Actor->new();
      if(length($mesh_name)) {
        $actor->name($mesh_name);
      } else {
        $actor->name(sprintf("ACTOR%d",$file_idx));
      }

      my @mesh_matches;
      my $mesh_match;
      my @tokens;

      @mesh_matches = grep( /^PARENT /  , @mesh_properties);
      $mesh_match = $mesh_matches[0];
      @tokens = split(' ', $mesh_match);
      $mesh_match = $tokens[1];
      if($mesh_match) {
        $actor->parent_name($mesh_match);
      }

      @mesh_matches = grep( /^IS_CAMERA /  , @mesh_properties);
      $mesh_match = $mesh_matches[0];
      @tokens = split(' ', $mesh_match);
      $mesh_match = $tokens[1];
      if($mesh_match) {
        $actor->is_camera($mesh_match);
      }

      @mesh_matches = grep( /^MESH /  , @mesh_properties);
      $mesh_match = $mesh_matches[0];
      @tokens = split(' ', $mesh_match);
      $mesh_match = $tokens[1];
      if($mesh_match) {
        $actor->meshfile($mesh_match);
      }

      @mesh_matches = grep( /^TEXCOORDS /  , @mesh_properties);
      $mesh_match = $mesh_matches[0];
      @tokens = split(' ', $mesh_match);
      $mesh_match = $tokens[1];
      if($mesh_match) {
        $actor->uvfile($mesh_match);
      }

      @mesh_matches = grep( /^TEXTURE /  , @mesh_properties);
      $mesh_match = $mesh_matches[0];
      @tokens = split(' ', $mesh_match);
      $mesh_match = $tokens[1];
      if($mesh_match) {
        $actor->texturefile($mesh_match);
      }

      @mesh_matches = grep( /^PLUGIN /  , @mesh_properties);
      $mesh_match = $mesh_matches[0];
      @tokens = split(' ', $mesh_match);
      $mesh_match = $tokens[1];
      if($mesh_match) {
        $actor->pluginfile($mesh_match);
      }

      my $loc = $extracted_info[1];
      my $or_row0 = $extracted_info[2];
      my $or_row1 = $extracted_info[3];
      my $or_row2 = $extracted_info[4];
      $or_row0 =~ s/\n//;
      $or_row1 =~ s/\n//;
      $or_row2 =~ s/\n//;
      $loc =~ s/\n//;

      my @loc_parts = split(' ',$loc);
      $loc = join(" ", $loc_parts[0], $loc_parts[2], $loc_parts[1]);

      # to change orientation from rhs to lhs we must do two things:
      # 1) swap the y and z columns in the matrix 
      # 2) swap the y and z rows in the matrix 
      my @row_parts = split(' ', $or_row0);
      $or_row0 = join(" ", $row_parts[0], $row_parts[2], $row_parts[1]);
      @row_parts = split(' ', $or_row1);
      $or_row1 = join(" ", $row_parts[0], $row_parts[2], $row_parts[1]);
      @row_parts = split(' ', $or_row2);
      $or_row2 = join(" ", $row_parts[0], $row_parts[2], $row_parts[1]);

      $actor->position($loc);
      $actor->orientation(join(" ", $or_row0, $or_row2, $or_row1));
      push @actors, $actor;
    } 
    else # elsif(lc($mesh_type) eq "sector")
    {

      ########################################################################
      # all untagged meshes are treated as sectors
      ########################################################################

      my $sector = Sector->new();

      if(length($mesh_name)) {
        $sector->name($mesh_name);
      } else {
        $sector->name(sprintf("S%d",$file_idx));
      }
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
  
        # blender/videoscape uses a +z-up RHS system; we use a +y-up LHS system.
        $vertex->x($list[0]);
        $vertex->z($list[1]);
        $vertex->y($list[2]);
      }
  
      # read in facet definitions from file
  
      my @face_properties = grep( /^FACE /  , @mesh_properties);
      print STDERR "Mesh properties: -------------\n";
      print STDERR @mesh_properties;
      print STDERR "Face properties: -------------\n";
      print STDERR @face_properties;

      my $facet_number = 0;
      while ( <FILE> ) {
        my @list = split(' ');
  
        if( $list[0] < 3 ) {
          next; # this "polygon" has less than three edges: either it is
                # a special ID flag, or it is a modeling error, but it
                # certainly is not geometry we need to write to the final
                # world file
        }

        pop @list; # pop color off list
        shift @list; # don't care about vertex index count

        my $facet = Facet->new($sector);
        $facet->set_is_invisible(0); # may be changed below
        # we add the facet to the sector object LATER, after it is determined
        # if it is visible or not (if not it is just there to define a
        # texture orientation)
  
        my @this_face_properties =
          grep( /^FACE *$facet_number /, @face_properties);

        print STDERR @this_face_properties;

        # we count the vertex indices in reverse because blender/videoscape
        # normally store the vertices counterclockwise(rhs) whereas we use
        # clockwise (rhs)
  
        for(my $i=$#list; $i>=0; $i--) {
          $facet->add_vertex_index( $list[$i] );
        }

        my $tex_id;
        my $tex_image_name;
        my @matching_texspace_lines =
          grep( /_TEX_SPACE/ , @this_face_properties);
        if($#matching_texspace_lines > -1) {
          # a polygon should EITHER use OR define a texture space, not both. 
          # also, a polygon can't use or define multiple textures. so, we
          # just take the first matching line with _TEX_SPACE and extract the
          # texture id from it.
          my @matching_texspace_tokens =
            split(' ', $matching_texspace_lines[0]);

          # joining sector name with tex id name ensures uniqueness among
          # reused texture space names among sectors. not joining it with sector
          # name allows texture spaces to be reused across sectors, since
          # the texture space list is global in the world, not local in the sector.
          $tex_id = join(".", $sector->name(), $matching_texspace_tokens[3]);
          $tex_image_name = $matching_texspace_tokens[4];
        }

        if ( grep( /IS_TEX_SPACE/ , @this_face_properties ) 
        &&   !grep( /IS_INVISIBLE/, @this_face_properties ) )
        {
          if ($facet->get_vertex_index_count() != 2) {
            printf(STDERR
                   "Non-triangle marked as geometry and tex-def-tri. ID: %s.\n",
                   $tex_id);
          } else {
            printf(STDERR "Geometry and tex-def-tri found. ID: %s\n", $tex_id);
            $facet->set_tex_id($tex_id);

            $self->get_world()->set_texture_imagenum($tex_image_name, "dummy");
            # we just insert the image name as the hash KEY into the hash;
            # the value, which is the numerical index, is assigned later
            # in l3dGen after all tex images have been defined.
            # using a hash ensures uniqueness among the keys.
    
            my $tex = Texture->new();
            $tex->set_tex_id($tex_id);
            $tex->set_image($tex_image_name);
            $tex->set_origin($facet->get_tex_origin());
            $tex->set_u_vec($facet->get_tex_u_vec());
            $tex->set_v_vec($facet->get_tex_v_vec());
            $self->get_world()->add_texture(sprintf("%s",$tex_id), $tex);
          }
        } elsif ( grep( /IS_TEX_SPACE/ , @this_face_properties ) 
             &&   grep( /IS_INVISIBLE/, @this_face_properties ) )
        {
          if ($facet->get_vertex_index_count() != 2) {
            printf(STDERR
                   "Non-triangle marked as pure tex-def-tri. ID: %s.\n",
                   $tex_id);
          } else {
            printf(STDERR
                   "Pure tex-def-tri found. ID: %s\n", $tex_id);
            $facet->set_is_invisible(1); # face not to be written to world file
            $facet->set_tex_id($tex_id);

            $self->get_world()->set_texture_imagenum($tex_image_name, "dummy");
            # we just insert the image name as the hash KEY into the hash;
            # the value, which is the numerical index, is assigned later
            # in l3dGen after all tex images have been defined.
            # using a hash ensures uniqueness among the keys.
    
            my $tex = Texture->new();
            $tex->set_tex_id($tex_id);
            $tex->set_image($tex_image_name);
            $tex->set_origin($facet->get_tex_origin());
            $tex->set_u_vec($facet->get_tex_u_vec());
            $tex->set_v_vec($facet->get_tex_v_vec());
            $self->get_world()->add_texture(sprintf("%s",$tex_id), $tex);
          }
        } elsif ( grep( /USES_TEX_SPACE/ , @this_face_properties ) ) { 
          printf(STDERR "Geometry poly num %d linked to tex ID: %s\n",
                 $facet_number, $tex_id);
          $facet->set_tex_id($tex_id);
        } else {
          printf(STDERR "Normal Geometry poly num %d\n", $facet_number);
        }
  
        # if facet was not invisible, add it to the world file
        if($facet->get_is_invisible() == 0) {
          $sector->add_facet( sprintf("%d",$facet_number), $facet );
        }
        $facet_number++;
      }
    }
  }

  # link portals together

  my $aSector_key;
  my $aSector;
  for $aSector_key ( keys % { $self->get_world()->get_all_sectors() } ) {
    $aSector = $self->get_world()->get_sector($aSector_key);
    $aSector->make_portals_from_holes();
  }
  $self->get_world()->link_portals();

  # assign actors to sectors

  print STDERR "Now assigning ", $#actors + 1, " actors to their sectors.\n";
  my $i;
  for ( $i = 0; $i<=$#actors; $i++) {
    print STDERR "  Assigning actor ", $actors[$i]->name(), " to sector ";
    print STDERR $actors[$i]->parent_name(), "...";
    $aSector = $self->get_world()->get_sector
      ($actors[$i]->parent_name());
    if($aSector) {
      $aSector->add_actor($actors[$i]);
      printf(STDERR "done\n");
    } else {
      printf(STDERR
             "\n  uh oh, couldnt't find parent sector... object discarded.\n");
    }
  }
}

1; # so the require or use succeeds
