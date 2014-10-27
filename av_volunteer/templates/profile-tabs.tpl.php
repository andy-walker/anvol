<a href="/volunteer/my-profile/<?php print $target_uid; ?>" class="btn btn-info btn-profile-info">Contact</a>
<a href="/volunteer/my-profile/details/<?php print $target_uid; ?>" class="btn btn-info btn-profile-info">Details</a>

<?php if (av_role_has_roles_inherit('Individual Volunteer')): ?>
  <a href="/volunteer/my-profile/skills/<?php print $target_uid; ?>" class="btn btn-info btn-profile-info">Skills</a>
  <a href="/volunteer/my-profile/references/<?php print $target_uid; ?>" class="btn btn-info btn-profile-info">References</a>
<?php endif; ?>