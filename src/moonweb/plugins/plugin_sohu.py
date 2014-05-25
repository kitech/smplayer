#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import re
import json
from utils import convert_to_utf8, list_links
import moonplayer
    
##--------##
## search ##
##--------##
def search(kw, page):
    url = 'http://so.tv.sohu.com/mts?box=1&wd=' + kw + '&p=' + str(page)
    moonplayer.get_url(url, search_cb, kw)
    
def search_cb(page, kw):
    page = convert_to_utf8(page)
    #tv series
    links = list_links(page, 'http://so.tv.sohu.com/show/', kw)
    n_series = len(links) / 2
    #movies
    links += list_links(page, 'http://tv.sohu.com/2', kw)
    n = len(links) / 2
    #others
    links += list_links(page, 'http://my.tv.sohu.com/us/', kw)
    #show
    moonplayer.show_list(links)
    for i in xrange(n_series):
        moonplayer.set_list_item_color(i, '#0000ff')
    for i in xrange(n_series, n):
	moonplayer.set_list_item_color(i, '#6a4100')
    
##-------##
## parse ##
##-------##
def parse(url, options):
    if url.startswith('http://so.tv.sohu.com/show/'): #tv series
        moonplayer.get_url(url, parse_tv_cb, None)
    elif url.startswith('http://tv.sohu.com/2'): #videos
        moonplayer.get_url(url, parse_cb, options)
    elif url.startswith('http://my.tv.sohu.com/us/'):
	vid = url.split('/')[-1].split('.')[0]
	url = 'http://my.tv.sohu.com/play/videonew.do?af=1&out=0&g=8&vid=' + vid
	moonplayer.get_url(url, parse_my_cb, [vid, options])
    else:
        moonplayer.warn('Wrong url')
        
## parse videos
vid_re = re.compile(r'vid\s?=\s?"(\d+)"')
plid_re = re.compile(r'playlistId="(\d*)"')
def parse_cb(page, options):
    vid_match = vid_re.search(page)
    plid_match = plid_re.search(page)
    if vid_match and plid_match:
        vid = vid_match.group(1)
        plid = plid_match.group(1)
        msg = [vid, plid, options]
        url = 'http://hot.vrs.sohu.com/vrs_flash.action?out=0&g=8&r=1&vid=%s&plid=%s' % (msg[0], msg[1])
        moonplayer.get_url(url, parse_cb2, msg)
    else:
        moonplayer.warn('Fail')

def parse_cb2(page, msg):
    page = json.loads(page)
    data = page[u'data']
    #try other quality
    try:
        if not data:
            moonplayer.warn('解析失败！换个清晰度试试？')
            return
        elif msg[2] & moonplayer.OPT_QL_SUPER and u'superVid' in data:
            newvid = data[u'superVid']
        elif msg[2] & (moonplayer.OPT_QL_HIGH | moonplayer.OPT_QL_SUPER):
            newvid = data[u'highVid']
        else:
            newvid = data[u'norVid']
        if int(newvid) > 0 and str(newvid) != msg[0]:
            msg[0] = str(newvid)
            url = 'http://hot.vrs.sohu.com/vrs_flash.action?out=0&g=8&r=1&vid=%s&plid=%s' % (msg[0],msg[1])
            moonplayer.get_url(url, parse_cb2, msg)
            return
    except KeyError:
        pass
    #parse
    clipsURL = data[u'clipsURL']
    su = data[u'su']
    ip = page[u'allot']
    name = data[u'tvName'].encode('UTF-8')
    result = []
    i = 0
    while i < len(clipsURL):
        url = u'http://' + ip + u'/?prot=' + clipsURL[i] + u'&new=' + su[i]
        url = url.encode('UTF-8')
        result.append(name + '_' + str(i) + '.mp4')
        result.append(url)
        i += 1
    if msg[2] & moonplayer.OPT_DOWNLOAD:
        moonplayer.download(result, result[0])
    else:
        moonplayer.play(result)
    
# parse personal videos
def parse_my_cb(page, msg):
    page = json.loads(page)
    data = page[u'data']
    #try other quality
    try:
        if not data:
            moonplayer.warn('解析失败！换个清晰度试试？')
            return
        elif msg[1] & moonplayer.OPT_QL_SUPER and u'superVid' in data:
            newvid = data[u'superVid']
        elif msg[1] & (moonplayer.OPT_QL_HIGH | moonplayer.OPT_QL_SUPER):
            newvid = data[u'highVid']
        else:
            newvid = data[u'norVid']
        if int(newvid) > 0 and str(newvid) != msg[0]:
            msg[0] = str(newvid)
            url = 'http://my.tv.sohu.com/play/videonew.do?af=1&out=0&g=8&vid=' + newvid
            moonplayer.get_url(url, parse_my_cb, msg)
            return
    except KeyError:
        pass
    #parse
    clipsURL = data[u'clipsURL']
    su = data[u'su']
    ip = page[u'allot']
    name = data[u'tvName'].encode('UTF-8')
    result = []
    i = 0
    while i < len(clipsURL):
        url = u'http://' + ip + u'/?prot=' + clipsURL[i] + u'&new=' + su[i]
        url = url.encode('UTF-8')
        result.append(name + '_' + str(i) + '.mp4')
        result.append(url)
        i += 1
    if msg[1] & moonplayer.OPT_DOWNLOAD:
        moonplayer.download(result, result[0])
    else:
        moonplayer.play(result)
    
# parse tv series
link_re = re.compile(r'<a target=_blank href=\s*["\'](http://tv[^"\']+?)["\']\s*>([^<>]+?)</a>')
def parse_tv_cb(page, options):
    links = list_links(page, 'http://tv.sohu.com/2')
    moonplayer.show_album(links)
    