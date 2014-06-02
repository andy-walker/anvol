/**
 * js for skills form treeview control - andyw@circle, 08/05/2014
 */
(function() {

    jQuery(function($) {
  
        $('#tree-container').jstree({
            "plugins": [ "checkbox" ],
        });

        $('#add-skills-link').click(function(event) {
            // show / hide skills popup when link clicked
            $('#add-skills-link').html($('#skills-popup').is(':visible') ? '+ Add skills' : '- Hide');
            $('#skills-popup').fadeToggle(300);
            // make this link not fire the event handler defined below 
            // (when clicking outside the popup), or strange stuff will happen
            event.stopPropagation();
        });

        // when clicked outside skills popup, hide skills popup
        $(document).click(function() {
            // hide skills popup when we lose focus
            if ($('#skills-popup').is(':visible')) {
                $('#add-skills-link').html('+ Add skills');
                $('#skills-popup').fadeOut(300);
            }
        });

        $("#skills-popup").click(function(event) {
            event.stopPropagation();
        });

        // when the 'Add' button is clicked, add the relevent skills to the list
        $('#skills-popup button').click(function() {
          
            var nextFree   = Drupal.settings.skills.nextFreeSlot,
                skillsList = Drupal.settings.skills.list, 
                skillz     = [];

            // not the recommended way to get checked items, but the recommended way doesn't work - 
            // nor does anything else for that matter ..
            $('#tree-container').find(".jstree-clicked").each(function(i, element) {
                // will return parent items as well - we're not interested in those, 
                // so check id is in list of skills passed into Drupal.settings
                skill = $(element).parent().attr("id");
                if (skill in skillsList)
                    skillz.push(skill);
            
            });  
            
            // iterate through selected skills, add to main form            
            skillz.forEach(function(skill, index) {
                $('.skill-wrapper-' + (nextFree + index))
                    .animate({ height: 'toggle', opacity: 'toggle' }, 300)
                    .find('.skill-label')
                    .html(skillsList[skill]);
            });

            Drupal.settings.skills.nextFreeSlot += skillz.length;
            $('#add-skills-link').html('+ Add skills');
            $('#skills-popup').fadeOut(300);           

            // prevent submission
            return false;
        
        });

        // attach event handlers to delete buttons
        $('.skill-wrapper button').each(function() {
            $(this).click(function() {
                // todo: probably some other stuff, but just hide for now
                $(this).parent().animate({ height: 'toggle', opacity: 'toggle' }, 300);
                return false;
            });         
        });

    });

}).call(this);