// javascript for My Profile -> Contact tab - andyw@circle, 24/05/2014
(function() {

    var current_status = 0;

    jQuery(function($) {
        
        // when status is 'Cancelled', 'Unavailable', 'Resigned',
        // grey out + disable availability checkbox table       
        $('input[name="status"]').change(function() {
            
            var new_status = $(this).val();

            if (new_status == 4 && current_status != 4) {
                
                $('.checkbox-table, .best-time-label').css({
                    opacity: 0.4
                }).find('input')
                  .attr('disabled', 'disabled');

            } else if (new_status != 4 && current_status == 4) {
                
                $('.checkbox-table, .best-time-label').css({
                    opacity: 1
                }).find('input')
                  .removeAttr('disabled');
            
            }

            current_status = new_status;

        });
    
    });

}).call(this);