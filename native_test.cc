#include "lispy.hh"

extern "C" void test_func(ValPtr& ret, const std::vector<ValPtr>& params)
{
	for(auto v : params)
		std::cout << to_string(v) << std::endl;		/**< 输出每个参数 */
	ret = std::make_shared<Val>("native test success");	/**< 返回字符串"native test success" */
}
