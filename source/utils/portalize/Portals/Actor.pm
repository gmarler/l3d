package Actor;
use strict;
use Portals::Facet;
use Portals::Portal;
use Portals::Vertex;

############################
## the object constructor ##
############################
sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{_name}   = undef;
    $self->{_parent} = undef;
    $self->{_parent_name} = undef;
    $self->{_orientation} = undef;
    $self->{_position}  = undef; 
    $self->{_is_camera}  = undef; 
    $self->{_meshfile}  = undef;
    $self->{_uvfile} = undef;
    $self->{_texturefile} = undef;
    $self->{_pluginfile}  = undef;

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

sub parent_name {
    my $self = shift;
    if (@_) { $self->{_parent_name} = shift }

    return $self->{_parent_name};
}

sub parent {
    my $self = shift;
    if (@_) { $self->{_parent} = shift }

    return $self->{_parent};
}

sub is_camera {
    my $self = shift;
    if (@_) { $self->{_is_camera} = shift }

    return $self->{_is_camera};
}

sub meshfile {
    my $self = shift;
    if (@_) { $self->{_meshfile} = shift }

    return $self->{_meshfile};
}

sub uvfile {
    my $self = shift;
    if (@_) { $self->{_uvfile} = shift }

    return $self->{_uvfile};
}

sub texturefile {
    my $self = shift;
    if (@_) { $self->{_texturefile} = shift }

    return $self->{_texturefile};
}

sub pluginfile {
    my $self = shift;
    if (@_) { $self->{_pluginfile} = shift }

    return $self->{_pluginfile};
}

sub orientation {
    my $self = shift;
    if (@_) { $self->{_orientation} = shift }

    return $self->{_orientation};
}

sub position {
    my $self = shift;
    if (@_) { $self->{_position} = shift }

    return $self->{_position};
}



1;  # so the require or use succeeds
