// All sizes are millimeters

// Global definitions
screw_cylinder_height = 8;
screw_cylinder_inner = 3;
screw_cylinder_outer = 4.5;

// Main plate 
surface_plate_width = 260;
surface_plate_height = 178;
surface_plate_thickness = 1.6;
surface_plate_xyz = [surface_plate_width,surface_plate_height, surface_plate_thickness];

// Display
display_width = 30;
display_height = 14;
display_thickness = 2;
display_size = [display_width, display_height, display_thickness];
display_distance_from_center_x = -30;
display_distance_from_center_y = 50;

// Volume knob
volume_knob_dia = 7;
volume_knob_position_x = (surface_plate_width / 2) - (volume_knob_dia / 2);
volume_knob_position_y = (surface_plate_height / 2) - (volume_knob_dia / 2);

// Button cuts
button_width = 13;
button_height = 13;
button_thickness = 2;
button_size = [button_width, button_height, button_thickness];
button_group_short_distance = 3;
button_group_big_distance = 13.55;
button_group_width_total = button_width * 4 + button_group_short_distance * 2 + button_group_big_distance;
button_group_position_x = (surface_plate_width / 2) - button_group_width_total / 2;
button_group_position_y = ((surface_plate_height / 2) - button_height / 2) + 50;

difference() {
    // Draw main plate
    cube(surface_plate_xyz);
    
    // Substract display
    translate ([surface_plate_width/2 - display_width /2 - 
    display_distance_from_center_x, 
    surface_plate_height/2 - display_height/2 - 
    display_distance_from_center_y])
    {
        cube(display_size);
    }
    
    // Substract volume knob
    translate([volume_knob_position_x, volume_knob_position_y]){
        cylinder(r=volume_knob_dia, h=2);
    }
    
    // Buttons
    translate([button_group_position_x, button_group_position_y])
    {
        cube(button_size);
    }
    
    translate([button_group_position_x + button_group_short_distance + button_width, button_group_position_y])
    {
        cube(button_size);
    }
    
    translate([button_group_position_x + button_group_big_distance + button_width * 2, button_group_position_y])
    {
        cube(button_size);
    }
    
    translate([button_group_position_x + button_group_short_distance + button_group_big_distance + button_width * 3, button_group_position_y])
    {
        cube(button_size);
    }
}

// draw screw holders display
/*translate ([surface_plate_width/2 - display_width /2 - 
    display_distance_from_center_x - 6, 
    surface_plate_height/2 - display_height/2 - 
    display_distance_from_center_y - 5]){
    difference(){
    cylinder(r=6, h=screw_cylinder_height);
        translate([screw_cylinder_outer/2 - screw_cylinder_inner /2]){
            cylinder(r=3, h=screw_cylinder_height);
        }
    }
}

translate ([surface_plate_width/2 - display_width /2 - 
    display_distance_from_center_x - 6, 
    surface_plate_height/2 + display_height/2 - 
    display_distance_from_center_y + 4]){
    difference(){
    cylinder(r=6, h=screw_cylinder_height);
        translate([screw_cylinder_outer/2 - screw_cylinder_inner /2]){
            cylinder(r=3, h=screw_cylinder_height);
        }
    }
}

translate ([surface_plate_width/2 + display_width /2 - 
    display_distance_from_center_x + 4, 
    surface_plate_height/2 + display_height/2 - 
    display_distance_from_center_y + 5]){
    difference(){
    cylinder(r=6, h=screw_cylinder_height);
        translate([screw_cylinder_outer/2 - screw_cylinder_inner /2]){
            cylinder(r=3, h=screw_cylinder_height);
        }
    }
}

translate ([surface_plate_width/2 + display_width /2 - 
    display_distance_from_center_x + 4, 
    surface_plate_height/2 - display_height/2 - 
    display_distance_from_center_y - 5]){
    difference(){
    cylinder(r=6, h=screw_cylinder_height);
        translate([screw_cylinder_outer/2 - screw_cylinder_inner /2]){
            cylinder(r=3, h=screw_cylinder_height);
        }
    }
}*/