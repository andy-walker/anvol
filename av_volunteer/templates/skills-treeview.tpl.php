<?php 

/**
 * output a hierarchical unordered list which will be transformed into treeview by jstree library
 * andyw@circle, 21/04/2014
 */

?>
<div id="add-skills">
  <a id="add-skills-link">+ Add skills</a>
  <div id="skills-popup">
    <div id="tree-container">
      <ul>
        <?php foreach ($skills->top_level as $tid => $top_level_label): ?>
          <li id="skill<?php print $tid; ?>"><?php print $top_level_label; ?>
          <?php if ($skills->sub_level[$tid]): ?>
            <ul>
              <?php foreach ($skills->sub_level[$tid] as $sub_tid => $sub_level_label): ?>
                <li id="skill<?php print $sub_tid; ?>"><?php print $sub_level_label; ?></li>
              <?php endforeach; ?>
            </ul>
          <?php endif; ?>
          </li>
        <?php endforeach; ?>
      </ul>
    </div>
    <div class="button-wrapper">
      <button class="btn-xs btn-success btn">Add »</button>
    </div>
  </div>
</div>