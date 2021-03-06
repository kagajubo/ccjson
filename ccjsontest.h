//
//  ccjsontest.h
//  cjson
//
//  Created by JerryZhou on 15/5/29.
//  Copyright (c) 2015 JerryZhou. All rights reserved.
//

#ifndef cjson_ccjsontest_h
#define cjson_ccjsontest_h

#include "simpletest.h"
#include "ccjsonstruct.h"
#include "ccjson.h"
#include <limits.h>

#define print printf 

#define _open_mem_trace (0) 

#if _open_mem_trace
// new memory begin
void membegin(const char* tag) {
    if (tag) {
        print("**********************%s**********************\n", tag);
    }
    cc_mem_cache_clear();
    cc_mem_cache_state();
    cc_mem_state();
}
#else
void membegin(const char* tag) {
    (void)tag;
}
#endif

SP_SUIT(ccjson);

SP_CASE(ccjson, ccarraymalloc) {
    SP_TRUE(1);
}

SP_CASE(ccjson, eg0) {
    config_app app = {0};
    membegin("eg0-begin");
    cc_enablememorycache(ccino);

    char *json = cc_read_file("app.json");

    size_t m1 = cc_mem_state();
    print("parse 1000 times about app json\n");
    for (int i=0; i<1000; ++i) {
        ccparsefrom(cctypeofmetaget(config_app), &app, json); 
    }
    size_t m2 = cc_mem_state();

    print("parse anthor 1000 times about app json\n");
    for (int i=0; i<1000; ++i) {
        ccparsefrom(cctypeofmetaget(config_app), &app, json); 
    }
    size_t m3 = cc_mem_state();
    SP_TRUE(m2 >= m1);
    SP_EQUAL(m2, m3);
    
    cc_free(json);

    char *unjson = ccunparseto(cctypeofmetaget(config_app), &app);

    print("unjson: %s\n", unjson);

    cc_free(unjson);

    ccobjrelease(cctypeofmetaget(config_app), &app);

    SP_TRUE(1);

    cc_enablememorycache(cciyes);

    membegin("eg0");
}

SP_CASE(ccjson, eg1) {
    config_date date = {0};
    date.invaliddate = (char*)("2015-05-11");
    date.validdate = (char*)("2015-05-31");
    ccobjset(&date, cctypeofmindex(config_date, invaliddate));
    ccobjset(&date, cctypeofmindex(config_date, validdate));

    char *unjson = ccunparseto(cctypeofmetaget(config_date), &date);
    print("unjson: %s\n", unjson);

    cc_write_file(unjson, "date.json");
    cc_free(unjson);

    SP_EQUAL(ccobjmcount(cctypeofmetaget(config_date)), 2);

    membegin("eg1");
}

SP_CASE(ccjson, eg2) {
    config_date *date = iccalloc(config_date);

    char * json = cc_read_file("date.json");
    iccparse(date, json);

    print("date: %s - %s\n", date->invaliddate, date->validdate);

    cc_free(json);

    iccfree(date);

    SP_TRUE(1);

    membegin("eg1");
}

SP_CASE(ccjson, eg3) {
    ccconfig *config = iccalloc(ccconfig);
    const char* json = "{\"ver\":1, \"has\":true, \"noexits\": 1,  \"detail\":\"no details\", \"skips\":[1, 2, 3]}";

    iccparse(config, json);
    print("config->ver: %d\n", config->ver);
    print("config->has: %d\n", config->has);
    SP_EQUAL(config->ver, 1);
    SP_EQUAL(config->has, cciyes);
    SP_EQUAL(strcmp(config->detail, "no details") , 0);

        SP_EQUAL(config->skips[0], 1);
        SP_EQUAL(config->skips[1], 2);
        SP_EQUAL(config->skips[2], 3);

    const char* json1 = "{\"ver\":2, \"has\":\"xxx\", \"noexits\": 1,  \"detail\":\"no details\", \"skips\":[0, 1, 2, 3]}";

    iccparse(config, json1);
    SP_EQUAL(config->ver, 2);
    SP_EQUAL(config->has, cciyes);
    SP_EQUAL(strcmp(config->detail, "no details") , 0);
    SP_EQUAL(config->skips[0], 0);
        SP_EQUAL(config->skips[1], 1);
        SP_EQUAL(config->skips[2], 2);
        SP_EQUAL(config->skips[3], 3);

    const char* json2 = "{\"ver\":3, \"has\":false, \"noexits\": 1,  \"detail\":\"no details\", \"skips\":true}";
    iccparse(config, json2);
    print("config->ver: %d\n", config->ver);
    print("config->has: %d\n", config->has);

    SP_EQUAL(config->ver, 3);
    SP_EQUAL(config->has, ccino);
    SP_EQUAL(strcmp(config->detail, "no details") , 0);
    SP_EQUAL(config->skips[0], 0);
        SP_EQUAL(config->skips[1], 1);
        SP_EQUAL(config->skips[2], 2);
        SP_EQUAL(config->skips[3], 3);

    iccfree(config);

    membegin("eg3");
}

SP_CASE(ccjson, eg4) {
    ccconfig *config = iccalloc(ccconfig);
    const char* json = "{\"ver\":1, \"has\":null, \"noexits\": 1,  \"detail\":\"no details\", \"skips\":[1, null, 2]}";
    print("json %s\n", json);

    iccparse(config, json);
    SP_EQUAL(config->ver, 1);
    SP_TRUE(ccobjhas(config, cctypeofmindex(ccconfig, ver)));
    SP_TRUE(!ccobjhas(config, cctypeofmindex(ccconfig, has)));
    SP_TRUE(ccobjhas(config, cctypeofmindex(ccconfig, detail)));
    SP_TRUE(ccobjhas(config, cctypeofmindex(ccconfig, skips)));

    char* unjson = iccunparse(config);
    print("unjson %s\n", unjson);

    const char* xjson = "{\"ver\":[1, 2, 3], \"has\":null, \"noexits\": 1,  \"detail\":\"no details\", \"skips\":1}";

    iccparse(config, xjson);
    print("unjson %s\n", xjson);
    SP_EQUAL(config->ver, 1);
    SP_TRUE(ccobjhas(config, cctypeofmindex(ccconfig, skips)));
    SP_TRUE(ccobjisnull(config, cctypeofmindex(ccconfig, has)));

    print("i m here skips %d !!\n", ccobjhas(config, cctypeofmindex(ccconfig, skips)));
    print("i m here skips len %zu !!\n", ccarraylen(config->skips));

    SP_EQUAL(config->skips[0], 1);
    SP_EQUAL(config->skips[1], 0);
    SP_EQUAL(config->skips[2], 2);

    iccrelease(config);

    // release all member
    SP_TRUE(!ccobjhas(config, cctypeofmindex(ccconfig, ver)));
    SP_TRUE(!ccobjhas(config, cctypeofmindex(ccconfig, has)));
    SP_TRUE(!ccobjhas(config, cctypeofmindex(ccconfig, detail)));
    SP_TRUE(!ccobjhas(config, cctypeofmindex(ccconfig, skips)));

    print("i m here 002 !!\n");

    iccfree(config);
    iccfree(unjson);

    SP_TRUE(1);

    membegin("eg4");
}

SP_CASE(ccjson, int64) {
    ccconfig *config = iccalloc(ccconfig);
    config->ver64 = -1000 * (ccint64)INT_MAX;
    ccobjset(config, cctypeofmindex(ccconfig, ver64));
    char *unjson = iccunparse(config);
    print("unjson: %s\n", unjson);

    ccconfig *nconfig = iccalloc(ccconfig);
    iccparse(nconfig, unjson);
    SP_EQUAL(nconfig->ver64, config->ver64);

    iccfree(unjson);
    iccfree(config);
    iccfree(nconfig);
}

SP_CASE(ccjson, point) {
    
    ccinittypemeta(cctypeofmetaget(test_json));
    ccinittypemeta(cctypeofmetaget(test_json_sub));
    
    cc_enablememorycache(ccino);

    size_t current = cc_mem_size();
    {
        const char* json = "{\"ip\":4}";
        test_json *test = iccalloc(test_json);
        
        iccparse(test, json);
        
        SP_EQUAL(*test->ip, 4);
        
        iccfree(test);
        
        size_t xcurrent = cc_mem_size();
        
        SP_EQUAL(xcurrent, current);

    }
    {
        const char* json = "{\"ip\":4, \"isub\":{\"i\":3}, \"xsub\":{\"i\":12}}";
        test_json *test = iccalloc(test_json);
        
        iccparse(test, json);
        
        SP_EQUAL(*test->ip, 4);
        
        SP_EQUAL(test->isub.i, 3);
        
        SP_EQUAL(test->xsub->i, 12);
        
        iccfree(test);
        
        size_t xcurrent = cc_mem_size();
        
        SP_EQUAL(xcurrent, current);    // check if we have memory leak
    }
    
    {
        const char* json = "{\"ip\":4, \"isub\":{\"i\":3}, \"xsub\":{\"i\":12}, \"array\":[1,2,3], \"subarray\":[{\"i\":10}, {\"i\":20}, {\"i\":30}]}";
        test_json *test = iccalloc(test_json);
        
        for (int i=0; i < 100; ++i) {
            iccparse(test, json);
        }
        
        SP_EQUAL(*test->ip, 4);
        
        SP_EQUAL(test->isub.i, 3);
        
        SP_EQUAL(test->xsub->i, 12);
        
        SP_EQUAL(test->array[0], 1);
        
        SP_EQUAL(test->array[1], 2);
        
        SP_EQUAL(test->array[2], 3);
        
        SP_EQUAL(test->subarray[0].i, 10);
        SP_EQUAL(test->subarray[1].i, 20);
        SP_EQUAL(test->subarray[2].i, 30);
        
        iccfree(test);
        
        size_t xcurrent = cc_mem_size();
        
        SP_EQUAL(xcurrent, current);    // check if we have memory leak
    }

    SP_TRUE(1);
    
    size_t xcurrent = cc_mem_size();

    SP_EQUAL(xcurrent, current);    // check if we have memory leak

    cc_enablememorycache(cciyes);
}

SP_CASE(ccjson, arraymalloc) {
    int *array = (int*)ccarraymalloc(3, sizeof(int), 0);
    
    ccarrayfree(array);
    ccjson_obj *arrayobj = ccarrayobj(array);
    char c0 = ccjsonobjhas(arrayobj, 0);
    char c1 = ccjsonobjhas(arrayobj, 1);
    
    SP_EQUAL(c0, 0);
    SP_EQUAL(c1, 0);
    
    ccarrayset(array, 0);
    
    char cc0 = ccjsonobjhas(arrayobj, 0);
    char cc1 = ccjsonobjhas(arrayobj, 1);
    
    SP_EQUAL(cc0, 1);
    SP_EQUAL(cc1, 0);
    
    ccarraysetnull(array, 1);
    
    char ccc0 = ccjsonobjhas(arrayobj, 0);
    char ccc1 = ccjsonobjhas(arrayobj, 1);
    
    SP_EQUAL(ccc0, 1);
    SP_EQUAL(ccc1, 2);
}

SP_CASE(ccjson, array) {
    ccconfig *config = iccalloc(ccconfig);
    const char* json = "{\"skips\":[1, null, 2]}";
    iccparse(config, json);
    char *unjson = iccunparse(config);
    
    ccibool has0 = ccarrayhas(config->skips, 0);
    ccibool has1 = ccarrayhas(config->skips, 1);
    ccibool has2 = ccarrayhas(config->skips, 2);
    
    ccibool null0 = ccarrayisnull(config->skips, 0);
    ccibool null1 = ccarrayisnull(config->skips, 1);
    ccibool null2 = ccarrayisnull(config->skips, 2);
    
    iccparse(config, unjson);
    
    ccibool xhas0 = ccarrayhas(config->skips, 0);
    ccibool xhas1 = ccarrayhas(config->skips, 1);
    ccibool xhas2 = ccarrayhas(config->skips, 2);
    
    ccibool xnull0 = ccarrayisnull(config->skips, 0);
    ccibool xnull1 = ccarrayisnull(config->skips, 1);
    ccibool xnull2 = ccarrayisnull(config->skips, 2);
    
    SP_EQUAL(has0, xhas0);
    SP_EQUAL(has1, xhas1);
    SP_EQUAL(has2, xhas2);
    
    SP_EQUAL(null0, xnull0);
    SP_EQUAL(null1, xnull1);
    SP_EQUAL(null2, xnull2);
    
    SP_EQUAL(has0, cciyes);
    SP_EQUAL(has1, ccino);
    SP_EQUAL(has2, cciyes);
    
    SP_EQUAL(null0, ccino);
    SP_EQUAL(null1, cciyes);
    SP_EQUAL(null2, ccino);
    
    iccfree(unjson);
    iccfree(config);
}

SP_CASE(ccjson, ccarrayfree) {
    int *array = (int*)ccarraymalloc(3, sizeof(int), 0);

    array[0] = 1;
    ccarrayset(array, 0);

    array[1] = 2;
    ccarrayset(array, 1);

    array[2] = 3;
    ccarrayset(array, 2);

    SP_EQUAL(ccarraylen(array), 3);

    print("array [0] = %d\n", ccarrayhas(array, 0));
    print("array [1] = %d\n", ccarrayhas(array, 1));
    print("array [2] = %d\n", ccarrayhas(array, 2));

    SP_EQUAL(ccarrayhas(array, 0), cciyes);
    SP_EQUAL(ccarrayhas(array, 1), cciyes);
    SP_EQUAL(ccarrayhas(array, 2), cciyes);

    iccfree(array);
}

SP_CASE(ccjson, cc_mem_cache) {
    cc_enablememorycache(cciyes);
    cc_mem_cache_clear();
    cc_mem_cache_state();
    SP_EQUAL(cc_mem_cache_current(1), 0);
    char * content = cc_dup("12345678");
    SP_EQUAL(cc_mem_cache_current(1), 0);

    iccfree(content);
    SP_EQUAL(cc_mem_cache_current(1), 1);
    content = cc_dup("12345678");
    SP_EQUAL(cc_mem_cache_current(1), 0);
    iccfree(content);
    SP_EQUAL(cc_mem_cache_current(1), 1);
    cc_mem_cache_clearof(1);
    SP_EQUAL(cc_mem_cache_current(1), 0);

    SP_EQUAL(cc_mem_cache_capacity(1), 100);
    cc_mem_cache_setcapacity(1, 3);
    char *alls[8] = {0};
    for (int i=0; i<5; ++i) {
        alls[i] = cc_dup("12345678"); 
    }
    SP_EQUAL(cc_mem_cache_current(1), 0);
    iccfree(alls[0]);
    SP_EQUAL(cc_mem_cache_current(1), 1);
    iccfree(alls[1]);
    SP_EQUAL(cc_mem_cache_current(1), 2);
    iccfree(alls[2]);
    SP_EQUAL(cc_mem_cache_current(1), 3);
    iccfree(alls[3]);
    SP_EQUAL(cc_mem_cache_current(1), 3);
    iccfree(alls[4]);
    SP_EQUAL(cc_mem_cache_current(1), 3);

    cc_mem_cache_state();

    cc_mem_cache_clearof(1);
    SP_EQUAL(cc_mem_cache_current(1), 0);

    cc_mem_cache_state();

    SP_TRUE(1);
}

SP_CASE(ccjson, benchmarktestcomplex) {
    config_app *app = iccalloc(config_app);
    char *json = cc_read_file("app.json");
    ccint64 cur = ccgetcurnano();
    for (int i=0; i<10000; ++i) {
        iccparse(app, json);
    }
    ccint64 since = ccgetcurnano() - cur;
    print("Parase 10000 Json Obj Take %lld nanos(%.6fs)\n", since, 1.0 *since/1000/1000);
    iccfree(json);
    iccfree(app);
    SP_TRUE(1);
}

SP_CASE(ccjson, havefun) 
//void nouse()
{
    print("have fun begin \n");
#define aw 10
#define ah 1000
    int (*a)[ah] = (int (*)[ah])malloc(sizeof(int)*aw*ah);
ccint64 cur = 0;
    print("have fun begin got memory\n");
    cur = ccgetcurnano();
    for (int j=0; j<ah; ++j) {
        for (int i=0; i<aw; ++i) {
            a[i][j] = i * j;
        }
    }
    print("10000 * 100 takes %lld nanos\n", ccgetcurnano() - cur);

    cur = ccgetcurnano();
    for (int i=0; i<aw; ++i) {
        for (int j=0; j<ah; ++j) {
            a[i][j] = i * j;
        }
    }
    print("100 * 10000 takes %lld nanos\n", ccgetcurnano() - cur);
}

SP_CASE(ccjson, benchmarktestcomplexdisablememcache) {
    cc_enablememorycache(ccino);
    config_app *app = iccalloc(config_app);
    char *json = cc_read_file("app.json");
    ccint64 cur = ccgetcurnano();
    for (int i=0; i<10000; ++i) {
        iccparse(app, json);
    }
    ccint64 since = ccgetcurnano() - cur;
    print("Parase 10000 Json Obj Take %lld nanos(%.6fs)\n", since, 1.0 *since/1000/1000);
    iccfree(json);
    iccfree(app);
    cc_enablememorycache(cciyes);
    SP_TRUE(1);
}

SP_CASE(ccjson, benchmarktest) {
    ccconfig *config = iccalloc(ccconfig);

    const char* json = "{\"ver\":1, \"has\":null, \"noexits\": 1,  \"detail\":\"no details\", \"skips\":[1, null, 2]}";
    ccint64 cur = ccgetcurnano();
    for (int i=0; i<10000; ++i) {
        iccparse(config, json);
    }
    ccint64 since = ccgetcurnano() - cur;
    print("Parase 10000 Json Obj Take %lld nanos(%.6fs)\n", since, 1.0 *since/1000/1000);
    iccfree(config);
    cc_mem_cache_clear();
    cc_mem_state();

    SP_TRUE(1);
}


#endif
