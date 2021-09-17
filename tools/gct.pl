#!/usr/bin/perl

#use strict;
#use Cwd;
#use Env;

my $aline;
my $lineid;
my $length;
my $address;
my @bytes;
my $addstr;
my $chsum=0;
my $count=0;
my $firstime=1;
my $i;
my $currentaddr;
my $tmp;
my $holder="";
my $loadaddr;

if(@ARGV < 2){
	die("\n Syntax: perl gct.pl uart_ddr_settings.conf u-boot.srec u-boot.asc\n");
}

open(IN_UART_DDR_SETTINGS, "<$ARGV[0]") || die("failed to open uart_ddr_settings.conf\n");
open(IN_UART_SREC, "<$ARGV[1]") || die("failed to open u-boot.srec\n");
open(OUT_UBOOT_ASC, ">$ARGV[2]") || die("failed to open u-boot.asc\n");

$i=0;
while ($line = <IN_UART_DDR_SETTINGS>){
	if($line=~/\w/){
		if($line!~/[;#\*]/){
			if($i eq 0){
				printf OUT_UBOOT_ASC ("33333333");
			}
			chomp($line);
			$line=~s/\t//;
			@array=split(/ +/,$line);
			$j=0;
			while(@array[$j]!~/\w/){
				$j=$j+1;
			}
			$addr=@array[$j];
			$regval=@array[$j+1];
			$addr=~s/0x//;
			$regval=~s/0x//;
			printf OUT_UBOOT_ASC ("%08x%08x",hex($addr),hex($regval));
			$i=$i+1;
			if($i eq 8){
				$i=0;
				printf OUT_UBOOT_ASC ("\n");
			}
		}
	}
}

while($i lt 8 && $i gt 0){
	printf OUT_UBOOT_ASC "00"x8;
	$i=$i+1;
}

if($i eq 8){
	printf OUT_UBOOT_ASC ("\n");
}

while($aline=<IN_UART_SREC>){
	$aline=uc($aline);
	chomp($aline);
	next if(($aline=~/^S0/) || ($aline=~/^S7/));
	($lineid, $length, $address, @bytes) = unpack"A2A2A8"."A2"x300, $aline;
	$length = hex($length);
	$address = hex($address);
	$length -=5;
	$i=0;

	while($length>0){
		if($firstime==1){
			$addstr = sprintf("%x", $address);
			$addstr = "0"x(8-length($addstr)).$addstr;
			print OUT_UBOOT_ASC $addstr;
			addchsum($addstr);
			$firstime=0;
			$currentaddr=$address;
			$loadaddr = $addstr;
		}
		else{
			if($count==64){
				$addstr = sprintf("%x", $currentaddr);
				$addstr = "0"x(8-length($addstr)).$addstr;
				print OUT_UBOOT_ASC $addstr;
				addchsum($addstr);
				$count=0;
			}
#printf("*** %x != %x\n", $address, $currentaddr) if $address != $currentaddr;
		}
		if($currentaddr < $address) {
			print OUT_UBOOT_ASC "00";
			addchsum("00");
			$count++;
			$currentaddr++;
		}
		else {
			while($count<64){
				$bytes[$i]=~tr/ABCDEF/abcdef/;
				print OUT_UBOOT_ASC "$bytes[$i]";
				addchsum($bytes[$i]);
				$i++;
				$count++;
				$currentaddr++;
				$length--;
				last if($length==0);
			}
		}
		if($count==64){
			print OUT_UBOOT_ASC "\n";
		}
	}
}
if($count != 64){
	$tmp = "00";
	for($i=0;$i<(64-$count);$i++){
		print OUT_UBOOT_ASC "00";
		addchsum($tmp);
	}
	print OUT_UBOOT_ASC "\n";
}


print OUT_UBOOT_ASC "11"x4;
use integer;
$chsum=$chsum & 0xffffffff;
$chsum = sprintf("%X", $chsum);
$chsum = "0"x(8-length($chsum)).$chsum;
$chsum =~tr/ABCDEF/abcdef/;
print OUT_UBOOT_ASC $chsum;
print OUT_UBOOT_ASC "00"x60;
print OUT_UBOOT_ASC "\n";

print OUT_UBOOT_ASC "99"x4;
print OUT_UBOOT_ASC $loadaddr;
print OUT_UBOOT_ASC "00"x60;
print OUT_UBOOT_ASC "\n";

close OUT_UBOOT_ASC;

sub addchsum{
	my $cc=$_[0];
	$holder=$holder.$cc;
	if(length($holder)==8){
		$holder = hex($holder);
		$chsum+=$holder;
		$holder="";
	}
}
