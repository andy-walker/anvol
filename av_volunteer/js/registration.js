// javascript for user registration form - andyw@circle, 28/05/2014
(function() {

    jQuery(function($) {

        // hide charity field on initial load
        $('.form-item-charity-name').css({"height":"0", "opacity":"0"});
        
        // when charity selected, show additional 'Charity Name' field    
        $('input[name="signup_type"]').change(function() {
            //console.log('test');
            $('.form-item-charity-name').animate(
                $(this).val() == 'volunteer' ? { height:0, opacity:0 } : { height:'4.5em', opacity:1 },
                { duration: 500 }
            );
        });
    
    });

}).call(this);