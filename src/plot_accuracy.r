if(!require('ggplot2')) install.packages("ggplot2"); library('ggplot2')
if(!require('cowplot')) install.packages("cowplot"); library('cowplot')

args <- commandArgs(trailingOnly = TRUE)
method <- unlist(strsplit(args[1], "_"))[2]
data <- read.csv(args[1], header = FALSE)

if (args[2]){
  colnames(data) <- c("threshold","kmer","smer", "fp", "fn")
  data$threshold <- as.factor(data$threshold)
  data$fn <- as.numeric(data$fn)
  data$fp <- as.numeric(data$fp)
  
  p <- ggplot(data, aes(threshold)) + labs(y = NULL) + 
    theme(legend.key.height= unit(0.4, 'cm')) +
    geom_line(aes(y=fn, group = 1, color = "False Negatives"))+
    geom_line(aes(y=fp, group = 1, color = "False Positives"))
  
  ggsave(paste("plot_",method,"_accuracy_thresholds.pdf", sep=""), p)
} else {
  colnames(data) <- c("kmer","smer", "fp", "fn")
  if ( length(args) > 2 ) {
    data <- subset(data, data$kmer <= as.numeric(args[3]))
    data <- subset(data, data$smer <= as.numeric(args[4]))
  }
  data$kmer <- as.factor(data$kmer)
  data$smer <- as.factor(data$smer)
  data$fn <- as.numeric(data$fn)
  data$fp <- as.numeric(data$fp)
  
  kmer_fn <- ggplot(data, aes(kmer)) + labs(y = "False Negatives") + 
    theme(legend.key.height= unit(0.4, 'cm'))
  for (i in levels(data$smer)){
    kmer_fn <- kmer_fn + geom_line(data = subset(data, data$smer==i), aes(y=fn, group = 10, color = smer))
  }
  
  
  smer_fn <- ggplot(data, aes(smer))+ labs(y = "False Negatives")+ 
    theme(legend.key.height= unit(0.4, 'cm'))
  for (i in levels(data$kmer)){
    smer_fn <- smer_fn + geom_line(data = subset(data, data$kmer==i), aes(y=fn, group = 10, color = kmer))
  }
  
  
  
  kmer_fp <- ggplot(data, aes(kmer))+ labs(y = "False Positives")+ 
    theme(legend.key.height= unit(0.4, 'cm'))
  for (i in levels(data$smer)){
    kmer_fp <- kmer_fp + geom_line(data = subset(data, data$smer==i), aes(y=fp, group = 10, color = smer))
  }
  
  
  
  smer_fp <- ggplot(data, aes(smer))+ labs(y = "False Positives")+ 
    theme(legend.key.height= unit(0.4, 'cm'))
  for (i in levels(data$kmer)){
    smer_fp <- smer_fp + geom_line(data = subset(data, data$kmer==i), aes(y=fp, group = 10, color = kmer))
  }
  p <- plot_grid(kmer_fn, smer_fn, kmer_fp, smer_fp, 
                 labels = c("A", "B", "C", "D"),
                 ncol = 2, nrow = 2)
  
  
  ggsave(paste("plot_",method,"_accuracy_all.pdf", sep=""), p)
  ggsave(paste("plot_",method,"_accuracy_kmer_fn.pdf", sep=""), kmer_fn)
  ggsave(paste("plot_",method,"_accuracy_kmer_fp.pdf", sep=""), kmer_fp)
  ggsave(paste("plot_",method,"_accuracy_smer_fn.pdf", sep=""), smer_fn)
  ggsave(paste("plot_",method,"_accuracy_smer_fp.pdf", sep=""), kmer_fp)
}

