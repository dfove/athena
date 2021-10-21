/*
 * Copyright (c) 2021 dfove.com Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <RStream.hpp>
#include <string>
#include <iostream>

using namespace athena;
using namespace std;

void p(const RStream& rstream)
{
	printf("********************************\n");
	printf("%s\n", rstream.Data());
	printf("rstream's size is %ld\n", rstream.Size());
	printf("rstream's capacity is %ld\n", rstream.Capacity());
	printf("********************************\n");
}

int main()
{
	RStream x("Hello,world!!");
	printf("x's size is %ld\n", x.Size());
	printf("x's capacity is %ld\n", x.Capacity());
	int i = -100;
	RStream y(&i, sizeof(int));
	printf("y's size is %ld\n", y.Size());
	printf("y's capacity is %ld\n", y.Capacity());

/*append()*/
	x.Append("hello", sizeof(i));
	printf("%s\n", x.Data());
	printf("x's size is %ld\n", x.Size());
	printf("x's capacity is %ld\n", x.Capacity());
	x.Assign("leijianghua", sizeof("leijianghua"));
	printf("%s\n", x.Data());
	printf("x's size is %ld\n", x.Size());
	printf("x's capacity is %ld\n", x.Capacity());
	y.Assign(x);
	p(y);


	RStream a("xx123456hello, world!"), b("y123456");
	printf("compare a,b:%d\n", a.Compare(b));
	cout << "a is " << a.Data() << " a.size " << a.Size() << " a.capacity " << a.Capacity() << endl;
	char bufa[100];
	memset(bufa, 0, sizeof(bufa));
	a.Copy(bufa, 10);
	printf("bufa is %s\n", bufa);

	a.Erase(0, 8);
	cout << "a is " << a.Data() << " a.size " << a.Size() << " a.capacity " << a.Capacity() << endl;
	a.Insert(0, b);
	cout << "a is " << a.Data() << " a.size " << a.Size() << " a.capacity " << a.Capacity() << endl;
	a.Replace(1, 9, "ABCDEF", 0, 3);
	cout << "a is " << a.Data() << " a.size " << a.Size() << " a.capacity " << a.Capacity() << endl;
	a.Replace(4, 3, 'F');
	cout << "a is " << a.Data() << " a.size " << a.Size() << " a.capacity " << a.Capacity() << endl;
	a << '4' ;
	cout << "a is " << a.Data() << " a.size " << a.Size() << " a.capacity " << a.Capacity() << endl;
	
	RStream j("abc"), k("def");
	if ( j > k )
		cout << j.Data() << " is great than " << k.Data() << endl;
	else if ( j == k )
		cout << j.Data() << " is equal to " << k.Data() << endl;
	else
		cout << j.Data() << " is less than " << k.Data() << endl;
	return 0;
}
