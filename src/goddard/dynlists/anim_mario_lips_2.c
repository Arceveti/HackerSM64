#include <PR/ultratypes.h>

#include "macros.h"
#include "animdata.h"
#include "../gd_types.h"

static s16 animdata_mario_lips_2_1[][3] = {
    {  -97,  -3, -1771 }, {  -97,  -3, -1772 }, {  -97,  -3, -1772 }, {  -97,  -4, -1773 },
    {  -97,  -4, -1773 }, {  -97,  -4, -1774 }, {  -97,  -4, -1775 }, {  -97,  -4, -1776 },
    {  -97,  -4, -1777 }, {  -97,  -5, -1778 }, {  -97,  -5, -1779 }, {  -97,  -5, -1780 },
    {  -97,  -5, -1781 }, {  -97,  -5, -1781 }, {  -97,  -5, -1781 }, {  -97,  -5, -1781 },
    {  -97,  -5, -1781 }, {  -97,  -5, -1780 }, {  -97,  -5, -1778 }, {  -97,  -4, -1777 },
    {  -97,  -4, -1774 }, {  -97,  -3, -1771 }, {  -97,  -2, -1764 }, {  -97,   0, -1750 },
    {  -97,   2, -1733 }, {  -96,   5, -1714 }, {  -96,   8, -1698 }, {  -96,  10, -1686 },
    {  -96,  11, -1681 }, {  -96,  11, -1683 }, {  -96,  10, -1690 }, {  -96,   8, -1699 },
    {  -96,   6, -1711 }, {  -97,   4, -1723 }, {  -97,   2, -1736 }, {  -97,   0, -1748 },
    {  -97,  -1, -1759 }, {  -97,  -3, -1767 }, {  -97,  -3, -1771 }, {  -97,  -4, -1773 },
    {  -97,  -4, -1775 }, {  -97,  -4, -1777 }, {  -97,  -5, -1778 }, {  -97,  -5, -1779 },
    {  -97,  -5, -1780 }, {  -97,  -5, -1781 }, {  -97,  -5, -1781 }, {  -97,  -5, -1781 },
    {  -97,  -5, -1781 }, {  -97,  -5, -1781 }, {  -97,  -5, -1781 }, {  -97,  -5, -1781 },
    {  -97,  -5, -1780 }, {  -97,  -5, -1780 }, {  -97,  -5, -1779 }, {  -97,  -5, -1779 },
    {  -97,  -4, -1778 }, {  -97,  -4, -1777 }, {  -97,  -4, -1776 }, {  -97,  -4, -1776 },
    {  -97,  -4, -1775 }, {  -97,  -4, -1774 }, {  -97,  -4, -1774 }, {  -97,  -4, -1773 },
    {  -97,  -4, -1772 }, {  -97,  -3, -1772 }, {  -97,  -3, -1772 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1772 }, {  -97,  -4, -1773 }, {  -97,  -4, -1775 },
    {  -97,  -4, -1776 }, {  -97,  -4, -1777 }, {  -97,  -4, -1777 }, {  -97,  -4, -1777 },
    {  -97,  -4, -1778 }, {  -97,  -4, -1778 }, {  -97,  -4, -1778 }, {  -97,  -5, -1778 },
    {  -97,  -5, -1778 }, {  -97,  -5, -1778 }, {  -97,  -5, -1778 }, {  -97,  -5, -1778 },
    {  -97,  -5, -1778 }, {  -97,  -5, -1778 }, {  -97,  -4, -1778 }, {  -97,  -4, -1778 },
    {  -97,  -4, -1778 }, {  -97,  -4, -1777 }, {  -97,  -4, -1777 }, {  -97,  -4, -1777 },
    {  -97,  -4, -1776 }, {  -97,  -4, -1775 }, {  -97,  -4, -1774 }, {  -97,  -4, -1773 },
    {  -97,  -3, -1772 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1772 }, {  -97,  -4, -1773 }, {  -97,  -4, -1775 },
    {  -97,  -4, -1778 }, {  -97,  -5, -1781 }, {  -97,  -5, -1784 }, {  -98,  -6, -1788 },
    {  -98,  -7, -1791 }, {  -98,  -7, -1795 }, {  -98,  -8, -1799 }, {  -98,  -8, -1803 },
    {  -98,  -9, -1806 }, {  -98,  -9, -1809 }, {  -99, -10, -1811 }, {  -99, -10, -1813 },
    {  -99, -10, -1814 }, {  -99, -10, -1814 }, {  -99, -10, -1813 }, {  -99, -10, -1811 },
    {  -98,  -9, -1808 }, {  -98,  -9, -1804 }, {  -98,  -8, -1798 }, {  -98,  -7, -1791 },
    {  -97,  -5, -1782 }, {  -97,  -3, -1771 }, {  -95,   3, -1728 }, {  -91,  17, -1641 },
    {  -87,  33, -1542 }, {  -83,  46, -1460 }, {  -82,  52, -1426 }, {  -82,  50, -1438 },
    {  -84,  45, -1470 }, {  -86,  37, -1517 }, {  -88,  28, -1573 }, {  -91,  18, -1632 },
    {  -93,   9, -1689 }, {  -95,   1, -1737 }, {  -97,  -3, -1771 }, {  -98,  -7, -1792 },
    {  -98,  -9, -1804 }, {  -99, -10, -1810 }, {  -99, -10, -1811 }, {  -99, -10, -1809 },
    {  -99,  -9, -1807 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, { -100,  -9, -1805 }, { -100,  -9, -1804 }, { -100,  -9, -1804 },
    { -100,  -9, -1803 }, { -100,  -9, -1802 }, { -101,  -9, -1801 }, { -101,  -9, -1800 },
    { -101,  -9, -1799 }, { -101,  -8, -1798 }, { -102,  -8, -1797 }, { -102,  -8, -1796 },
    { -102,  -8, -1795 }, { -102,  -8, -1794 }, { -103,  -8, -1793 }, { -103,  -8, -1791 },
    { -103,  -7, -1790 }, { -103,  -7, -1789 }, { -103,  -7, -1787 }, { -103,  -7, -1786 },
    { -103,  -7, -1785 }, { -103,  -6, -1783 }, { -102,  -6, -1782 }, { -102,  -6, -1781 },
    { -102,  -6, -1779 }, { -101,  -5, -1778 }, { -101,  -5, -1777 }, { -100,  -5, -1776 },
    { -100,  -4, -1775 }, {  -99,  -4, -1773 }, {  -98,  -4, -1772 }, {  -97,  -3, -1771 },
    {  -95,  -3, -1770 }, {  -92,  -2, -1769 }, {  -88,  -1, -1769 }, {  -83,   0, -1768 },
    {  -78,   0, -1767 }, {  -72,   1, -1766 }, {  -67,   2, -1765 }, {  -62,   3, -1764 },
    {  -57,   4, -1763 }, {  -54,   5, -1762 }, {  -51,   6, -1761 }, {  -50,   6, -1759 },
    {  -48,   6, -1758 }, {  -47,   7, -1756 }, {  -46,   7, -1754 }, {  -45,   8, -1752 },
    {  -45,   8, -1749 }, {  -45,   8, -1747 }, {  -45,   8, -1745 }, {  -45,   8, -1743 },
    {  -45,   8, -1741 }, {  -46,   8, -1740 }, {  -47,   8, -1739 }, {  -48,   8, -1738 },
    {  -49,   8, -1738 }, {  -51,   8, -1738 }, {  -54,   7, -1739 }, {  -58,   6, -1742 },
    {  -63,   5, -1745 }, {  -68,   3, -1749 }, {  -74,   2, -1754 }, {  -80,   0, -1758 },
    {  -86,   0, -1763 }, {  -91,  -2, -1767 }, {  -94,  -3, -1770 }, {  -97,  -3, -1771 },
    {  -98,  -4, -1772 }, { -100,  -4, -1773 }, { -101,  -4, -1774 }, { -102,  -5, -1774 },
    { -103,  -5, -1774 }, { -104,  -5, -1774 }, { -104,  -5, -1774 }, { -105,  -5, -1773 },
    { -105,  -5, -1773 }, { -105,  -5, -1773 }, { -105,  -5, -1772 }, { -104,  -5, -1772 },
    { -104,  -5, -1771 }, { -103,  -5, -1771 }, { -102,  -4, -1771 }, { -101,  -4, -1771 },
    { -100,  -4, -1771 }, {  -99,  -4, -1771 }, {  -97,  -3, -1771 }, {  -92,  -3, -1773 },
    {  -84,  -1, -1775 }, {  -74,   0, -1777 }, {  -64,   1, -1780 }, {  -56,   3, -1782 },
    {  -52,   3, -1784 }, {  -50,   4, -1784 }, {  -49,   4, -1784 }, {  -48,   4, -1785 },
    {  -47,   4, -1785 }, {  -47,   4, -1785 }, {  -46,   4, -1785 }, {  -46,   4, -1785 },
    {  -47,   4, -1785 }, {  -47,   4, -1785 }, {  -47,   4, -1785 }, {  -48,   4, -1785 },
    {  -48,   4, -1784 }, {  -49,   4, -1784 }, {  -49,   4, -1784 }, {  -50,   4, -1784 },
    {  -50,   4, -1784 }, {  -51,   4, -1784 }, {  -51,   4, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 }, {  -52,   3, -1784 },
    {  -52,   3, -1785 }, {  -52,   3, -1787 }, {  -52,   3, -1791 }, {  -52,   2, -1795 },
    {  -52,   2, -1800 }, {  -52,   2, -1803 }, {  -52,   2, -1806 }, {  -52,   1, -1807 },
    {  -52,   2, -1806 }, {  -52,   2, -1802 }, {  -52,   2, -1795 }, {  -52,   3, -1784 },
    {  -51,   7, -1744 }, {  -49,  13, -1674 }, {  -47,  19, -1606 }, {  -46,  22, -1572 },
    {  -46,  22, -1570 }, {  -45,  22, -1576 }, {  -44,  21, -1589 }, {  -44,  20, -1606 },
    {  -44,  18, -1626 }, {  -44,  16, -1648 }, {  -45,  15, -1670 }, {  -47,  12, -1691 },
    {  -51,  10, -1709 }, {  -58,   7, -1730 }, {  -70,   2, -1755 }, {  -84,  -3, -1779 },
    {  -94,  -7, -1798 }, {  -99,  -9, -1805 }, { -100,  -9, -1806 }, { -100,  -9, -1807 },
    { -101, -10, -1808 }, { -101, -10, -1809 }, { -101, -10, -1810 }, { -102, -10, -1811 },
    { -102, -10, -1811 }, { -102, -10, -1812 }, { -103, -11, -1813 }, { -103, -11, -1813 },
    { -103, -11, -1814 }, { -104, -11, -1814 }, { -104, -11, -1815 }, { -104, -11, -1815 },
    { -104, -11, -1816 }, { -105, -11, -1816 }, { -105, -11, -1817 }, { -105, -12, -1817 },
    { -105, -12, -1817 }, { -105, -12, -1818 }, { -105, -12, -1818 }, { -106, -12, -1818 },
    { -106, -12, -1818 }, { -106, -12, -1818 }, { -106, -12, -1819 }, { -106, -12, -1819 },
    { -106, -12, -1819 }, { -106, -12, -1819 }, { -106, -12, -1819 }, { -106, -12, -1819 },
    { -106, -12, -1819 }, { -106, -12, -1819 }, { -106, -12, -1819 }, { -106, -12, -1819 },
    { -106, -12, -1819 }, { -106, -12, -1819 }, { -106, -12, -1819 }, { -106, -12, -1819 },
    { -106, -12, -1818 }, { -106, -12, -1818 }, { -106, -12, -1818 }, { -105, -12, -1818 },
    { -105, -12, -1818 }, { -105, -12, -1817 }, { -105, -12, -1817 }, { -105, -12, -1817 },
    { -105, -11, -1817 }, { -105, -11, -1817 }, { -105, -11, -1816 }, { -104, -11, -1816 },
    { -104, -11, -1816 }, { -104, -11, -1815 }, { -104, -11, -1815 }, { -104, -11, -1815 },
    { -104, -11, -1815 }, { -104, -11, -1814 }, { -103, -11, -1814 }, { -103, -11, -1814 },
    { -103, -11, -1813 }, { -103, -11, -1813 }, { -103, -11, -1813 }, { -103, -11, -1812 },
    { -102, -10, -1812 }, { -102, -10, -1812 }, { -102, -10, -1811 }, { -102, -10, -1811 },
    { -102, -10, -1811 }, { -102, -10, -1810 }, { -101, -10, -1810 }, { -101, -10, -1810 },
    { -101, -10, -1809 }, { -101, -10, -1809 }, { -101, -10, -1809 }, { -101, -10, -1809 },
    { -101, -10, -1808 }, { -100, -10, -1808 }, { -100, -10, -1808 }, { -100, -10, -1807 },
    { -100,  -9, -1807 }, { -100,  -9, -1807 }, { -100,  -9, -1807 }, { -100,  -9, -1807 },
    { -100,  -9, -1806 }, { -100,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1806 },
    {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1806 },
    {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 }, {  -99,  -9, -1806 },
    {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1806 },
    {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1806 },
    {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 },
    {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 },
    {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 },
    {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 }, {  -99,  -9, -1807 },
    {  -99,  -9, -1806 }, {  -99,  -9, -1806 }, {  -99,  -9, -1805 }, {  -99,  -9, -1805 },
    {  -99,  -9, -1804 }, {  -99,  -9, -1804 }, {  -99,  -9, -1803 }, {  -99,  -9, -1803 },
    {  -99,  -9, -1802 }, {  -99,  -8, -1801 }, {  -99,  -8, -1801 }, {  -99,  -8, -1800 },
    {  -99,  -8, -1799 }, {  -99,  -8, -1799 }, {  -99,  -8, -1798 }, {  -99,  -8, -1797 },
    {  -99,  -8, -1796 }, {  -98,  -7, -1795 }, {  -98,  -7, -1794 }, {  -98,  -7, -1794 },
    {  -98,  -7, -1793 }, {  -98,  -7, -1792 }, {  -98,  -7, -1791 }, {  -98,  -7, -1790 },
    {  -98,  -6, -1789 }, {  -98,  -6, -1788 }, {  -98,  -6, -1787 }, {  -98,  -6, -1787 },
    {  -98,  -6, -1786 }, {  -98,  -6, -1785 }, {  -98,  -5, -1784 }, {  -98,  -5, -1783 },
    {  -98,  -5, -1782 }, {  -98,  -5, -1781 }, {  -97,  -5, -1780 }, {  -97,  -5, -1780 },
    {  -97,  -5, -1779 }, {  -97,  -4, -1778 }, {  -97,  -4, -1777 }, {  -97,  -4, -1777 },
    {  -97,  -4, -1776 }, {  -97,  -4, -1775 }, {  -97,  -4, -1774 }, {  -97,  -4, -1774 },
    {  -97,  -4, -1773 }, {  -97,  -4, -1772 }, {  -97,  -3, -1772 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1770 }, {  -97,  -3, -1770 },
    {  -97,  -3, -1770 }, {  -97,  -3, -1770 }, {  -97,  -3, -1770 }, {  -97,  -3, -1770 },
    {  -97,  -3, -1770 }, {  -97,  -3, -1770 }, {  -97,  -3, -1770 }, {  -97,  -3, -1770 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
};

static s16 animdata_mario_lips_2_2[][3] = {
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 },
    {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1839 }, {  -96, -15, -1838 },
    {  -96, -15, -1837 }, {  -96, -14, -1836 }, {  -96, -14, -1835 }, {  -96, -14, -1834 },
    {  -96, -14, -1832 }, {  -96, -13, -1831 }, {  -96, -13, -1829 }, {  -96, -13, -1827 },
    {  -96, -13, -1825 }, {  -96, -12, -1824 }, {  -96, -12, -1822 }, {  -96, -12, -1820 },
    {  -96, -11, -1818 }, {  -96, -11, -1817 }, {  -96, -11, -1815 }, {  -96, -11, -1814 },
    {  -96, -10, -1812 }, {  -96, -10, -1810 }, {  -96, -10, -1808 }, {  -96,  -9, -1807 },
    {  -96,  -9, -1805 }, {  -96,  -9, -1803 }, {  -96,  -8, -1800 }, {  -96,  -8, -1798 },
    {  -96,  -8, -1796 }, {  -96,  -7, -1794 }, {  -96,  -7, -1792 }, {  -96,  -7, -1790 },
    {  -96,  -6, -1788 }, {  -96,  -6, -1786 }, {  -97,  -6, -1784 }, {  -97,  -5, -1782 },
    {  -97,  -5, -1780 }, {  -97,  -5, -1779 }, {  -97,  -4, -1777 }, {  -97,  -4, -1776 },
    {  -97,  -4, -1775 }, {  -97,  -4, -1774 }, {  -97,  -4, -1773 }, {  -97,  -3, -1772 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1772 }, {  -97,  -4, -1772 }, {  -97,  -4, -1773 },
    {  -97,  -4, -1774 }, {  -97,  -4, -1775 }, {  -97,  -4, -1776 }, {  -97,  -4, -1777 },
    {  -97,  -5, -1778 }, {  -97,  -5, -1779 }, {  -97,  -5, -1781 }, {  -97,  -5, -1782 },
    {  -96,  -6, -1784 }, {  -96,  -6, -1786 }, {  -96,  -6, -1787 }, {  -96,  -6, -1789 },
    {  -96,  -7, -1790 }, {  -96,  -7, -1792 }, {  -96,  -7, -1794 }, {  -96,  -8, -1796 },
    {  -96,  -8, -1797 }, {  -96,  -8, -1799 }, {  -96,  -8, -1801 }, {  -96,  -9, -1802 },
    {  -96,  -9, -1804 }, {  -96,  -9, -1805 }, {  -96,  -9, -1807 }, {  -96, -10, -1808 },
    {  -96, -10, -1809 }, {  -96, -10, -1810 }, {  -96, -10, -1811 }, {  -96, -10, -1812 },
    {  -96, -11, -1813 }, {  -96, -11, -1814 }, {  -96, -11, -1815 }, {  -96, -11, -1815 },
    {  -96, -11, -1815 }, {  -96, -11, -1815 }, {  -96, -11, -1815 }, {  -96, -11, -1815 },
    {  -96, -11, -1814 }, {  -96, -11, -1814 }, {  -96, -10, -1813 }, {  -96, -10, -1812 },
    {  -96, -10, -1811 }, {  -96, -10, -1810 }, {  -96, -10, -1808 }, {  -96,  -9, -1807 },
    {  -96,  -9, -1806 }, {  -96,  -9, -1804 }, {  -96,  -9, -1802 }, {  -96,  -8, -1801 },
    {  -96,  -8, -1799 }, {  -96,  -8, -1797 }, {  -96,  -7, -1795 }, {  -96,  -7, -1793 },
    {  -96,  -7, -1792 }, {  -97,  -7, -1790 }, {  -97,  -6, -1788 }, {  -97,  -6, -1786 },
    {  -97,  -6, -1784 }, {  -97,  -5, -1783 }, {  -97,  -5, -1781 }, {  -97,  -5, -1780 },
    {  -97,  -5, -1778 }, {  -97,  -4, -1777 }, {  -97,  -4, -1776 }, {  -97,  -4, -1775 },
    {  -97,  -4, -1774 }, {  -97,  -4, -1773 }, {  -97,  -4, -1772 }, {  -97,  -3, -1772 },
    {  -97,  -3, -1771 }, {  -97,  -3, -1771 },
};

struct AnimDataInfo anim_mario_lips_2[] = {
    { ARRAY_COUNT(animdata_mario_lips_2_1), GD_ANIM_ROT3S, animdata_mario_lips_2_1 },
    { ARRAY_COUNT(animdata_mario_lips_2_2), GD_ANIM_ROT3S, animdata_mario_lips_2_2 },
    END_ANIMDATA_INFO_ARR,
};
