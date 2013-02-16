#!/usr/bin/env perl

use warnings;
use strict;

print << "END_TOP";
#ifndef HAVE_COLOR_TABLE
#define HAVE_COLOR_TABLE
static const unsigned char color_table[] = {
END_TOP

while (<>) {
    if (my ($r, $g, $b) = m/^([0-9a-z][0-9a-z])([0-9a-z][0-9a-z])([0-9a-z][0-9a-z])$/i) {
        print "    0x$r, 0x$g, 0x$b,\n";
    }
}

print << "END_BOT";
};
static const size_t color_table_num_colors = sizeof(color_table) / (3 * sizeof(color_table[0]));
#endif /* HAVE_COLOR_TABLE */
END_BOT
