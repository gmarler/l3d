package Portal;

use Portals::Facet;
@ISA = qw(Facet);

sub get_target_sector {
  my $self = shift;
  return $self->{_target_sector};
}

sub set_target_sector {
  my $self = shift;
  my $target = shift;
  $self->{_target_sector} = $target;
}

1;
