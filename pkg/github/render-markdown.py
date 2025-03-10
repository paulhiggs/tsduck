#!/usr/bin/env python
#-----------------------------------------------------------------------------
#
#  TSDuck - The MPEG Transport Stream Toolkit
#  Copyright (c) 2005-2025, Thierry Lelegard
#  BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
#
#  Render a markdown file in the context of the TSDuck project.
#  The file is opened in a browser window.
#
#-----------------------------------------------------------------------------

import os, sys, time, tempfile, base64, html.parser, requests, webbrowser, tsgithub

# Get command line options.
repo = tsgithub.repository(sys.argv)

# A class which extracts all <link> to style sheets in the header of an HTML document.
class style_grabber(html.parser.HTMLParser):
    def __init__(self, html_text):
        super().__init__()
        self.head_depth = 0
        self.text = ''
        self.feed(html_text)
        self.close()
    def handle_starttag(self, tag, attrs):
        if tag == 'head':
            self.head_depth += 1
    def handle_endtag(self, tag):
        if tag == 'head':
            self.head_depth -= 1
    def handle_startendtag(self, tag, attrs):
        if tag == 'link' and self.head_depth > 0 and len([a for a in attrs if a[0] == 'rel' and a[1] == 'stylesheet']) > 0:
            self.text += self.get_starttag_text()

# Get HTML <link> to GitHub .css files which are required to display GitHub markdown.
html_begin = '<html><head>' + style_grabber(requests.get(repo.repo_url).text).text + '</head><body>'
html_end = '</body></html>'

# Open HTML text in a browser.
def open_html(text):
    # This is not as trivial as it seems.
    # 1. Previous versions used to build a "data URL" containing the HTML text.
    #      url = "data:text/html;base64," + base64.b64encode(text.encode()).decode('utf-8')
    #    However, for valid security reasons, some browsers such as Firefox no longer
    #    accept to open data URL's, unless they are explicitly entered by the user.
    # 2. Now create a temporary HTML file. However, on some distros such as Ubuntu,
    #    the Firefox snap refuses to open files in /tmp. Files must be in the user's
    #    directory tree. So, we need to change the temporary directory on some OS.
    tmpdir = None
    if os.name == 'posix':
        # Use/create a temporay directory in user's home.
        tmpdir = os.path.abspath(os.getenv('HOME', '/tmp') + '/tmp')
        if not os.path.isdir(tmpdir):
            os.mkdir(tmpdir)
    # Create the temporary file.
    fd, fname = tempfile.mkstemp(suffix='.html', dir=tmpdir, text=True)
    os.write(fd, text.encode())
    os.close(fd)
    # Open the HTML in a browser.
    url = 'file://' + fname if os.name == 'posix' else fname
    webbrowser.open_new_tab(url)
    # Delete the temporary files after a delay to let the browser load the file.
    if not repo.debug_mode:
        time.sleep(2)
        os.remove(fname)

# Loop on all markdown files.
for filename in repo.argv[1:]:
    with open(filename) as f:
        open_html(html_begin + repo.github.render_markdown(f.read(), context=repo.repo) + html_end)
