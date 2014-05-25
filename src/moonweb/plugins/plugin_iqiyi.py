#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import moonplayer
import json
from utils import list_links
try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
    
## hosts
hosts = ('www.iqiyi.com',)

## search    
def search(keyword, page):
    url = 'http://so.iqiyi.com/so/q_%s_ctg__t_0_page_%d_p_1_qc_0_rd__site_iqiyi' % (keyword, page)
    moonplayer.get_url(url, search_cb, keyword)
        
dianshiju_re = re.compile(r'http://www.iqiyi.com/dianshiju/[^/]+.html')
def search_cb(content, keyword):
    #tv series
    result = list_links(content, 'http://www.iqiyi.com/a_')
    result += list_links(content, dianshiju_re)
    n = len(result) / 2
    #movies
    result += list_links(content, 'http://www.iqiyi.com/dianying/')
    n2 = len(result) / 2
    #single video
    result += list_links(content, 'http://www.iqiyi.com/v_', keyword)
    moonplayer.show_list(result)
    for i in xrange(n):
        moonplayer.set_list_item_color(i, '#0000ff')
    for i in xrange(n, n2):
        moonplayer.set_list_item_color(i, '#ff00ff')
    
## parse
def parse(url, options):
    if dianshiju_re.match(url):
        moonplayer.get_url(url, parse_tv_cb, None)
    elif url.startswith('http://www.iqiyi.com/a_'):
        moonplayer.get_url(url, parse_a_cb, None)
    elif url.startswith('http://www.iqiyi.com/v_') or \
    url.startswith('http://www.iqiyi.com/dianshiju/') or \
    url.startswith('http://www.iqiyi.com/dianying/'):
        parser.feed(url, options)
    else:
        moonplayer.warn('Wrong URL!')
    
    
def parse_tv_cb(content, data):
    result = list_links(content, 'http://www.iqiyi.com/dianshiju/')
    moonplayer.show_album(result)
    

def parse_a_cb(content, data):
    result = list_links(content, 'http://www.iqiyi.com/v_')
    moonplayer.show_album(result)


vid_re = re.compile(r'data-player-videoid="(.+?)"')
tvid_re = re.compile(r'tvId:\s?(\d+),')
name_re = re.compile(r'data-sharethirdparty-videoname="(.+?)"')

class Parser(object):
    def __init__(self):
        pass
    
    def feed(self, url, options):
        moonplayer.get_url(url, self.parse_vid, options)
        
    def parse_vid(self, content, options):
        name_match = name_re.search(content)
        if name_match:
            self.name = name_match.group(1)
        else:
            self.name = '未知视频名称'
        tvid_match = tvid_re.search(content)
        vid_match = vid_re.search(content)
        if tvid_match and vid_match:
            tvid = tvid_match.group(1)
            vid = vid_match.group(1)
            url = 'http://cache.video.qiyi.com/vd/%s/%s/' % (tvid, vid)
            moonplayer.get_url(url, self.parse_vurl, options)
        else:
            moonplayer.warn('Fail!')
        
    def parse_vurl(self, content, options):
        # Read urls of all qualities
        root = json.loads(content)
        vs = root[u'tkl'][0][u'vs']
        bids = [None] * 5
        for v in vs:
            bid = int(v[u'bid']) - 1
            if bid < 5:
                bids[bid] = ['http://data.video.qiyi.com/videos' + item[u'l'] for item in v[u'fs']]
        # Select quality
        if options & moonplayer.OPT_QL_SUPER:
            q = 3
        elif options & moonplayer.OPT_QL_HIGH:
            q = 1
        else:
            q = 0
        for i in xrange(q, -1, -1):
            if bids[q]:
                self.urls = bids[q]
                break
        if options & moonplayer.OPT_DOWNLOAD and bids[4]:
            if moonplayer.question('是否下载1080P版本？'):
                self.urls = bids[4]
        
        # Get video's key
        first_uid = self.urls[0].split('/')[-1].split('.')[0]
        self.suffix = str(self.urls[0].split('.')[-1])
        url = 'http://data.video.qiyi.com/' + first_uid + '.ts'
        moonplayer.get_url(url, self.parse_key, options)
        
    def parse_key(self, content, options):
        result = []
        key = moonplayer.final_url.split('key=')[1]
        for i in xrange(len(self.urls)):
            name = '%s_%d.%s' % (self.name, i, self.suffix)
            result.append(name) #name
            result.append(self.urls[i] + '?key=' + key) #url
        
        if options & moonplayer.OPT_DOWNLOAD:
            moonplayer.download(result, result[0])
        else:
            if len(self.urls) > 2:
                moonplayer.warn('本视频比较长，由于奇异视频地址变化很频繁，建议下载后播放。')
            moonplayer.play(result)
        
parser = Parser()