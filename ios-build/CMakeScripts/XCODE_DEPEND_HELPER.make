# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.FDKK-linker.Debug:
/Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/Debug/libFDKK-linker.a:
	/bin/rm -f /Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/Debug/libFDKK-linker.a


PostBuild.FDKK-linker.Release:
/Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/Release/libFDKK-linker.a:
	/bin/rm -f /Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/Release/libFDKK-linker.a


PostBuild.FDKK-linker.MinSizeRel:
/Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/MinSizeRel/libFDKK-linker.a:
	/bin/rm -f /Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/MinSizeRel/libFDKK-linker.a


PostBuild.FDKK-linker.RelWithDebInfo:
/Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/RelWithDebInfo/libFDKK-linker.a:
	/bin/rm -f /Users/4dage_imacmini/Desktop/LEI_WORKPLACE/FDKKEyeLinkLib/ios-build/RelWithDebInfo/libFDKK-linker.a




# For each target create a dummy ruleso the target does not have to exist
