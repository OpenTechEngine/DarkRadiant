#ifndef DEFTOKENISER_H_
#define DEFTOKENISER_H_

#include "ParseException.h"

#include <string>
#include <iostream>

#include <boost/tokenizer.hpp>

namespace parser {

/* Model of boost::TokenizerFunction which splits tokens on whitespace with additional
 * protection of quoted content.
 */

class DefTokeniserFunc {
    
    // Enumeration of states
    enum {
        SEARCHING,        // haven't found anything yet
        TOKEN_STARTED,    // found the start of a possible multi-char token
        QUOTED,         // inside quoted text, no tokenising
        FORWARDSLASH,   // forward slash found, possible comment coming
        COMMENT_EOL,    // double-forwardslash comment
        COMMENT_DELIM,  // inside delimited comment (/*)
        STAR            // asterisk, possibly indicates end of comment (*/)
    } _state;
        
    // List of delimiters to skip
    const char* _delims;
    
    // List of delimiters to keep
    const char* _keptDelims;
    
    // Test if a character is a delimiter
    bool isDelim(char c) {
        const char* curDelim = _delims;
        while (*curDelim != 0) {
            if (*(curDelim++) == c) {
                return true;
            }
        }
        return false;
    }
    
    // Test if a character is a kept delimiter
    bool isKeptDelim(char c) {
        const char* curDelim = _keptDelims;
        while (*curDelim != 0) {
            if (*(curDelim++) == c) {
                return true;
            }
        }
        return false;
    }
    
    
public:

    // Constructor
    DefTokeniserFunc()
    : _state(SEARCHING), _delims(" \t\n\v\r"), _keptDelims("{}")
    {}

    /* REQUIRED. Operator() is called by the boost::tokenizer. This function
     * must search for a token between the two iterators next and end, and if
     * a token is found, set tok to the token, set next to position to start
     * parsing on the next call, and return true.
     */
    template<typename InputIterator, typename Token>
    bool operator() (InputIterator& next, InputIterator end, Token& tok) {

        // Initialise state, no persistence between calls
        _state = SEARCHING;

        // Clear out the token, no guarantee that it is empty
        tok = "";

        for ( ; next != end; next++) {
            
            switch (_state) {

                case SEARCHING:

                    // If we have a delimiter, just advance to the next character
                    if (isDelim(*next)) {
                        continue;
                    }

                    // If we have a KEPT delimiter, this is the token to return.
                    if (isKeptDelim(*next)) {
                        tok = *(next++);
                        return true;
                    }

                    // Otherwise fall through into TOKEN_STARTED, saving the state for the
                    // next iteration
                    _state = TOKEN_STARTED;

                case TOKEN_STARTED:


                    // Here a delimiter indicates a successful token match
                    if (isDelim(*next) || isKeptDelim(*next)) {
                        return true;
                    }

                    // Now next is pointing at a non-delimiter. Switch on this character.
                    switch (*next) {
                        
                        // Found a quote, enter QUOTED state, or return the current token if we
                        // are in the process of building one.
                        case '\"':
                            if (tok != "") {
                                return true;
                            }
                            else {
                                _state = QUOTED;
                                continue; // skip the quote
                            }
            
                        // Found a slash, possibly start of comment
                        case '/':
                            _state = FORWARDSLASH;
                            continue; // skip slash, will need to add it back if this is not a comment
            
                        // General case. Token lasts until next delimiter.
                        default:
                            tok += *next;
                            continue;
                    }
                    
                case QUOTED:
        
                    // In the quoted state, just advance until the closing quote. No
                    // delimiter splitting is required.
                    if (*next == '\"') {
                        if (tok != "") { // have a token now, close quote indicates the end
                            next++;
                            return true;
                        } 
                        else {
                            _state = SEARCHING;
                            continue; // do not add final quote
                        }
                    }
                    else {
                        tok += *next;
                        continue;
                    }
                        
                case FORWARDSLASH:
                
                    // If we have a forward slash we may be entering a comment. The forward slash
                    // will NOT YET have been added to the token, so we must add it manually if
                    // this proves not to be a comment.
                    
                    switch (*next) {
                        
                        case '*':
                            _state = COMMENT_DELIM;
                            continue;
                            
                        case '/':
                            _state = COMMENT_EOL;
                            continue;
                            
                        default: // false alarm, add the slash and carry on
                            _state = SEARCHING;
                            tok += "/";
                            next--; // defeat the for loop increment, to avoid skipping this char
                            continue;
                    }
                    
                case COMMENT_DELIM:
                
                    // Inside a delimited comment, we add nothing to the token but check for
                    // the "*/" sequence.
                    
                    if (*next == '*') {
                        _state = STAR;
                        continue;
                    }
                    else {
                        continue; // ignore and carry on
                    }

                case COMMENT_EOL:
                
                    // This comment lasts until the end of the line.
                    
                    if (*next == '\r' || *next == '\n') {
                        _state = SEARCHING;
                        continue;
                    }
                    else {
                        continue; // do nothing
                    }
                    
                case STAR:
                
                    // The star may indicate the end of a delimited comment. This state will
                    // only be entered if we are inside a delimited comment.
                    
                    if (*next == '/') {
                        _state = SEARCHING;
                        continue;
                    }
                    else {
                        continue; // do SEARCHING
                    }

            } // end of state switch
        } // end of for loop
        
        // Return true if we have added anything to the token
        if (tok != "")
            return true;
        else
            return false;
    }
    
    // REQUIRED. Reset function to clear internal state
    void reset() {
        _state = SEARCHING;
    }
    
};


/** Tokenise a DEF file.
 * 
 * This class provides a similar interface to Java's StringTokenizer class. It accepts
 * and std::string and a list of separators, and provides a simple interface to return
 * the next token in the string. It also protects quoted content and ignores both C and
 * C++ style comments.
 */

class DefTokeniser
{

    // Internal Boost tokenizer and its iterator
    typedef boost::tokenizer<DefTokeniserFunc> CharTokeniser;
    CharTokeniser _tok;
    CharTokeniser::iterator _tokIter;

public:

    /** Construct a DefTokeniser with the given input string, and optionally
     * a list of separators.
     * 
     * @param str
     * The string to tokenise.
     * 
     * @param seps
     * The list of characters to use as delimiters.
     */

    DefTokeniser(const std::string& str, const char* seps = " \t\n")
    : _tok(str, DefTokeniserFunc()),
      _tokIter(_tok.begin()) 
    {
    }
        
        
    /** Test if this StringTokeniser has more tokens to return.
     * 
     * @returns
     * true if there are further tokens, false otherwise
     */
     
    bool hasMoreTokens() {
        return _tokIter != _tok.end();
    }


    /** Return the next token in the sequence.
     * 
     * @returns
     * std::string containing the next token in the sequence.
     * 
     * @pre
     * hasMoreTokens() must be true, otherwise the result is undefined
     */
     
    const std::string nextToken() {
        if (hasMoreTokens())
            return *(_tokIter++);
        else
            throw ParseException("DefTokeniser: no more tokens");
    }
    
    
    /** Assert that the next token in the sequence must be equal to the provided
     * value. A ParseException is thrown if the assert fails.
     */
     
    void assertNextToken(const std::string& val) {
        const std::string tok = nextToken();
        if (tok != val)
            throw ParseException("DefTokeniser: Assertion failed\nRequired \"" + val + "\", found \"" + tok + "\"");
    }   
};

} // namespace parser

#endif /*DEFTOKENISER_H_*/
