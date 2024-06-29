#!/usr/bin/awk -f

NF>0 {
    if (saw[$0] != "") next;
    saw[$0] = "y";
}
{
    print;
}
