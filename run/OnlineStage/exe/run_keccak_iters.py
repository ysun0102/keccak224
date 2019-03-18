

import os
import sys



#############################################


st = 2000
iters = 10
gpu_id = 0
stride = 32
gpu_num = 1

#############################################


def main(start_pos, end_pos, gpu_id):

	filename = str('../data/logs_gpu/keccak_search_' + str(start_pos) + '_' + str(end_pos) + '.txt')

	if os.path.exists(filename):

		f = open(filename, 'r')
		st = int(f.readlines()[-1])
		f.close()
	else:
		st = start_pos
		f = open(filename, 'w')
		f.write('{}\n'.format(st))
		f.close()

	os.system('chmod 777 b2_keccak224r3')


	for p in range(st, end_pos):

		
		print('computing segment {}...'.format(p))
		res = os.popen('./b2_keccak224r3 {} {}'.format(p, gpu_id))		

		f = open(filename, 'a')
		f.write(res.read())
		f.write('{}\n'.format(p+1))
		f.close()






if __name__ == '__main__':
    for i in range(iters):
        main(st + i * stride * gpu_num + gpu_id * stride, st + i * stride * gpu_num + (gpu_id + 1) * stride, gpu_id)




