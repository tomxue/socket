#include <linux/module.h>
#include <linux/init.h>
#include <linux/in.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/inet.h>

#define SERVER_PORT 5555
static struct socket *recsocket=NULL;
static struct socket *sendsocket=NULL;

static DECLARE_COMPLETION( threadcomplete );
struct workqueue_struct *wq;

struct wq_wrapper{
    struct work_struct worker;
	struct sock * sk;
};

struct wq_wrapper wq_data;

//cb: callback
static void cb_data(struct sock *sk, int bytes){
	wq_data.sk = sk;
	queue_work(wq, &wq_data.worker);	//queue worker to wq
}

void send_answer(struct work_struct *data){
	struct  wq_wrapper * foo = container_of(data, struct  wq_wrapper, worker);
	int len = 0;
	/* as long as there are messages in the receive queue of this socket*/
	while((len = skb_queue_len(&foo->sk->sk_receive_queue)) > 0){
		struct sk_buff *skb = NULL;
		unsigned short * port;
		int len;
		struct msghdr msg;  //4.4BSD message passing
		struct iovec iov;
		mm_segment_t oldfs;
		struct sockaddr_in to;

		/* receive packet */
		skb = skb_dequeue(&foo->sk->sk_receive_queue);
		printk("message len: %i message: %s\n", skb->len - 8, skb->data+8); /*8 for udp header*/

		/* generate answer message */
		memset(&to,0, sizeof(to));
		to.sin_family = AF_INET;
		to.sin_addr.s_addr = in_aton("127.0.0.1");  
		port = (unsigned short *)skb->data; //data comes from the received packet and contains udp header: client port
		to.sin_port = *port;
		memset(&msg,0,sizeof(msg));
		msg.msg_name = &to;             //Socket name
		msg.msg_namelen = sizeof(to);
		/* send the message back */
		iov.iov_base = skb->data+8;
		iov.iov_len  = skb->len-8;
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		/* adjust memory boundaries */	
		oldfs = get_fs();
		set_fs(KERNEL_DS);
		len = sock_sendmsg(sendsocket, &msg, skb->len-8);
		set_fs(oldfs);
		/* free the initial skb */
		kfree_skb(skb);
	}
}

static int __init server_init( void )
{
	struct sockaddr_in server;
	int servererror;
	printk("INIT MODULE\n");
	/* create socket (recsocket) to receive data */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &recsocket) < 0) {
		printk( KERN_ERR "server: Error creating recsocket.n" );
		return -EIO;
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( (unsigned short)SERVER_PORT);
    //bind recsocket to server(INADDR_ANY, port: 5555)
	servererror = recsocket->ops->bind(recsocket, (struct sockaddr *) &server, sizeof(server));
	if (servererror) {
		sock_release(recsocket);
		return -EIO;
	}
    
    /* create socket to send data */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &sendsocket) < 0) {
		printk( KERN_ERR "server: Error creating sendsocket.n" );
		return -EIO;
	}

    /* create work queue */	
    wq = create_singlethread_workqueue("myworkqueue"); 
	if (!wq){
		return -ENOMEM;
	}
	INIT_WORK(&wq_data.worker, send_answer);    //queue send_answer because it will not be executed immediately

    //sk_data_ready: callback to indicate there is data to be processed
    //in the callback func: queue worker(connect func send_answer) to the wq
	recsocket->sk->sk_data_ready = cb_data;
	
	return 0;
}

static void __exit server_exit( void )
{
	if (recsocket)
		sock_release(recsocket);
	if (sendsocket)
		sock_release(sendsocket);

	if (wq) {
                flush_workqueue(wq);
                destroy_workqueue(wq);
	}
	printk("EXIT MODULE");
}

module_init(server_init);
module_exit(server_exit);
MODULE_LICENSE("GPL");
