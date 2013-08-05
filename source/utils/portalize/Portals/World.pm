package World;
use strict;
use Portals::Sector;
use Portals::Texture;

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_sectors}   = {};
    $self->{_textures}   = {};
    $self->{_texture_imagenums}   = {};
    $self->{_minx}   = 500000;
    $self->{_maxx}   = -500000;
    $self->{_miny}   = 500000;
    $self->{_maxy}   = -500000;
    $self->{_minz}   = 500000;
    $self->{_maxz}   = -500000;
    $self->{_nodesizex}  = 200;
    $self->{_nodesizey}  = 200;
    $self->{_nodesizez}  = 200;

    bless ($self, $class);
    return $self;
}

##############################################
## methods to access per-object data        ##
##                                          ##
## With args, they set the value.  Without  ##
## any, they only retrieve it/them.         ##
##############################################

sub get_nodesizex {
    my $self = shift;
    return $self->{_nodesizex};
}

sub get_nodesizey {
    my $self = shift;
    return $self->{_nodesizey};
}

sub get_nodesizez {
    my $self = shift;
    return $self->{_nodesizez};
}

sub get_minx {
    my $self = shift;
    return $self->{_minx};
}

sub get_miny {
    my $self = shift;
    return $self->{_miny};
}

sub get_minz {
    my $self = shift;
    return $self->{_minz};
}

sub get_maxx {
    my $self = shift;
    return $self->{_maxx};
}

sub get_maxy {
    my $self = shift;
    return $self->{_maxy};
}

sub get_maxz {
    my $self = shift;
    return $self->{_maxz};
}

sub set_minx {
    my $self = shift;
    my $v = shift;
    $self->{_minx} = $v;
}

sub set_miny {
    my $self = shift;
    my $v = shift;
    $self->{_miny} = $v;
}

sub set_minz {
    my $self = shift;
    my $v = shift;
    $self->{_minz} = $v;
}

sub set_maxx {
    my $self = shift;
    my $v = shift;
    $self->{_maxx} = $v;
}

sub set_maxy {
    my $self = shift;
    my $v = shift;
    $self->{_maxy} = $v;
}

sub set_maxz {
    my $self = shift;
    my $v = shift;
    $self->{_maxz} = $v;
}


sub get_nodesizex {
    my $self = shift;
    return $self->{_nodesizex};
}

sub get_nodesizey {
    my $self = shift;
    return $self->{_nodesizey};
}

sub get_nodesizez {
    my $self = shift;
    return $self->{_nodesizez};
}

sub get_all_sectors {
    my $self = shift;

    return $self->{_sectors};
}

sub get_sector {
    my $self = shift;
    my $sector_name = shift;

    return $self->{_sectors}->{$sector_name};
}

sub add_sector {
    my $self = shift;
    my $sector_name = shift;
    my $sector = shift;

    $self->{_sectors}->{$sector_name} = $sector;
}

sub get_all_textures {
    my $self = shift;

    return $self->{_textures};
}

sub get_texture {
    my $self = shift;
    my $tex_id = shift;

    return $self->{_textures}->{$tex_id};
}

sub add_texture {
    my $self = shift;
    my $tex_id = shift;
    my $tex = shift;

    $self->{_textures}->{$tex_id} = $tex;
}

sub get_all_texture_imagenums {
    my $self = shift;

    return $self->{_texture_imagenums};
}

sub get_texture_imagenum {
    my $self = shift;
    my $texi_id = shift;

    return $self->{_texture_imagenums}->{$texi_id};
}

sub set_texture_imagenum {
    my $self = shift;
    my $texi_id = shift;
    my $texi = shift;

    $self->{_texture_imagenums}->{$texi_id} = $texi;
}

sub link_portals {
  my $self = shift;

  my $aSector_key;
  my $aSector;
  my $aPortal_key;
  my $aPortal;
  my $aOtherSector_key;
  my $aOtherSector;
  my $aOtherPortal_key;
  my $aOtherPortal;
  for $aSector_key ( keys % { $self->get_all_sectors() } ) {
    my $unlinked_portal_num = 0;
    $aSector = $self->get_sector($aSector_key);

    print STDERR "Linking portals from Sector $aSector_key.\n";
    for $aPortal_key ( keys % { $aSector->get_all_portals() } ) {
      $aPortal = $aSector->get_portal($aPortal_key);
      if ($aPortal->get_target_sector()) {
        print STDERR "  Already assigned other side of portal $aPortal_key";
        print STDERR "(Sector ", $aPortal->get_target_sector()->name(), ").\n";
      } else {
        print STDERR "  Searching for other side of portal $aPortal_key.\n";

        my $portal_linked = undef;
        for $aOtherSector_key ( keys % { $self->get_all_sectors() } ) {
          if ($aOtherSector_key ne $aSector_key) {
            $aOtherSector = $self->get_sector($aOtherSector_key);
            for $aOtherPortal_key (keys %{$aOtherSector->get_all_portals()}) {
              $aOtherPortal = $aOtherSector->get_portal($aOtherPortal_key);
              if ( $aPortal->coincident_with_facet($aOtherPortal) ) {
                $portal_linked = 1;
                print STDERR "    Coincides with Sector $aOtherSector_key,";
                print STDERR " Portal $aOtherPortal_key\n";
                $aPortal->set_target_sector($aOtherSector);
                $aOtherPortal->set_target_sector($aSector); #reverse link
              }
            }
          }
        }
        if ( ! $portal_linked) {
          print STDERR "    No link found!\n";
          $aSector->delete_portal($aPortal);
          $aSector->add_nonlinked_portal(sprintf("%d",$unlinked_portal_num++),
                                         $aPortal);
        }
      }
    }
  }
}

1;  # so the require or use succeeds
