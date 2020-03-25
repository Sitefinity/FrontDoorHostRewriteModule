# FrontDoorHostRewriteModule (App Service extension)

# Installation
Install this extension using your App Service's SCM site.

# Description
Once installed, you will need to restart the site and the SCM site (through kudu). Afterward, all requests will be rewritten so the Host header matches the X-Forwarded-Host.

This extension limits itself to the App Service Slot it is being installed through. The extension аdds a global native C++ module to the App Service IIS that replaces the HOST header value with the value of X-Forwarded-Host header in case it exists.

# Notes
There is a popular approach to replace the HOST header with the value of X-Forwarded-Host header by unlocking the Host and X-Forwarded-Host headers, so they can be rewritten using rewrite rules. Once the request reaches the App Service, the value of the HOST header is set to the value of the X-Forwarded-Host header.
However, this approach has a pitfall. If there are other rewrite rules executed after the rule for rewriting the HOST header, they do not respect the new value of the HOST header.
Using the native IIS module ensures that the HOST header has taken the value of the X-Forwarded-Host header before the rewrite module steps in.
