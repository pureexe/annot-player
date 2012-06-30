// VLCConfig.h
// 9/3/2011

// Adapt objc with objc++

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wmissing-braces"     // missing braces around initializr
# pragma GCC diagnostic ignored "-Wparentheses"        // suggest parentheses around assignment used a truth value
# pragma GCC diagnostic ignored "-Wstrict-overflow"    // compmarison between signed and unsigned
//# pragma GCC diagnostic ignored "-Wstrict-overflow=3"
//# pragma GCC diagnostic ignored "-Wstrict-overflow=4"
//# pragma GCC diagnostic ignored "-Wstrict-overflow=5"
# pragma GCC diagnostic ignored "-Wunused-function"    // defined not used
# pragma GCC diagnostic ignored "-Wunused-parameter"   // unused parameter
#endif // __GNUC__

#ifdef __clang__
# pragma clang diagnostic ignored "-Wsemicolon-before-method-body" // semicolon before method body is ignored
# pragma clang diagnostic ignored "-Wsign-compare"     // comparison of integers of different signs
# pragma clang diagnostic ignored "-Wdeprecated-declarations"      // libvlc_track_description_release is deprecated
#endif // __clang__

// EOF
