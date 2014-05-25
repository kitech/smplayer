#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import moonplayer
import json
from utils import list_links

hosts = ('www.funshion.com',)

def search(keyword, page):
    url = 'http://www.funshion.com/search/media/page-%i.word-%s' % (page, keyword)
    moonplayer.get_url(url, search_cb, None)
    
sub_re = re.compile(r'<a href=["\']/subject/(\d+)/["\']>(.+?)</a>')
def search_cb(page, data):
    page = page.replace("<span class='keyword'>", '').replace('</span>', '')
    result = []
    match = sub_re.search(page)
    while match:
        (mid, name) = match.group(1, 2)
        url = 'http://www.funshion.com/subject/' + mid + '/'
        result.append(name)
        result.append(url)
        match = sub_re.search(page, match.end(0))
    moonplayer.show_list(result)

tv_re = re.compile(r'http://www.funshion.com/vplay/m-(\d+).e-(\d+)')
tv_re2 = re.compile(r'http://www.funshion.com/vplay/m-(\d+)')
def parse(url, options):
    if url.startswith('http://www.funshion.com/subject/'):
        vid = url.split('/')[4]
        url = 'http://api.funshion.com/ajax/get_web_fsp/' + vid + '/mp4'
        moonplayer.get_url(url, parse_subject, None)
        return
    
    match = tv_re.search(url)
    if match:
        (vid, num) = match.group(1, 2)
        url = 'http://api.funshion.com/ajax/get_web_fsp/' + vid + '/mp4'
        moonplayer.get_url(url, parse_m_vid, (options, int(num)))
        return
    
    match = tv_re2.search(url)
    if match:
        vid = match.group(1)
        url = 'http://api.funshion.com/ajax/get_web_fsp/' + vid + '/mp4'
        moonplayer.get_url(url, parse_m_vid, (options, 1))
        return
    moonplayer.warn('Wrong url.')
    

# Parse TV series list
def parse_subject(page, data):
    result = []
    mults = json.loads(page)[u'data'][u'fsps'][u'mult']
    for mult in mults:
        name = 'unknown'
        try:
            name = mult[u'name'].encode('UTF-8')
            name = mult[u'full'].encode('UTF-8')
        except KeyError:
            pass
        url = 'http://www.funshion.com' + str(mult[u'url'])
        result.append(name)
        result.append(url)
    moonplayer.show_album(result)
    
# Parse 'http://www.funshion.com/vplay/m-...'
def parse_m_vid(page, data):
    options = data[0]
    mults = json.loads(page)[u'data'][u'fsps'][u'mult']
    for mult in mults:
        if mult[u'number'] == data[1]:
            name = 'unknown'
            try:
                name = mult[u'name'].encode('UTF-8')
                name = mult[u'full'].encode('UTF-8')
            except KeyError:
                pass
            name += '.mp4'
            cid = str(mult[u'cid'])
            if options & moonplayer.OPT_QL_SUPER:
                p = 737280  # 720p
            elif options & moonplayer.OPT_QL_HIGH:
                p = 491520  # 480p
            else:
                p = 327680  # 320p
            url = 'http://jobsfe.funshion.com/query/v1/mp4/%s.json?bits=%i' % (cid, p)
            moonplayer.get_url(url, parse_m_cb, (options, name))
            
def parse_m_cb(page, data):
    url = json.loads(page)[u'playlist'][0][u'urls'][0]
    name = data[1]
    options = data[0]
    if options & moonplayer.OPT_DOWNLOAD:
        moonplayer.download([name, str(url)])
    else:
        moonplayer.play([name, str(url)])
